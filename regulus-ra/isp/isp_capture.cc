#include <gst/gst.h>

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <system_error>

namespace {

constexpr int kFrameCount = 5;
constexpr int kCaptureTimeoutSeconds = 90;
constexpr int kFullWidth = 1920;
constexpr int kFullHeight = 1080;
constexpr int kCenterCropSize = 512;

// Regulus-RA timing values for the IMX678 camera on CSI2-R0.
constexpr char kImx678Controls[] =
    "ipi_mode=1,hline=11852,hsa=10,hbp=10,hsd=10,"
    "vact=2160,vsa=10,vbp=15,vfp=15,"
    "ipi2dvp_hact=3840,ipi2dvp_hblank=30,"
    "ipi2dvp_vact=2160,ipi2dvp_vsa=296300,"
    "ipi2dvp_vbp=15,ipi2dvp_vfp=15,"
    "mipi_buffer_dummy_bytes=3840";

// Full mode is the baseline. Other modes change only the required fields.
struct CaptureMode {
    const char* description = "Full-size image";
    int output_width = kFullWidth;
    int output_height = kFullHeight;
    int subsampling = 0;
    int crop_left = 0;
    int crop_top = 0;
    int crop_right = 0;
    int crop_bottom = 0;
    int horizontal_flip = 0;
    int vertical_flip = 0;
    const char* output_name = "full";
};

CaptureMode MakeSubsampleHalfMode() {
    CaptureMode mode;
    mode.description = "1/2 image subsampling";
    mode.output_width = kFullWidth / 2;
    mode.output_height = kFullHeight / 2;
    mode.subsampling = 1;
    mode.output_name = "subsample_half";
    return mode;
}

CaptureMode MakeCenterCropMode() {
    CaptureMode mode;
    mode.description = "512x512 center crop";
    mode.output_width = kCenterCropSize;
    mode.output_height = kCenterCropSize;
    mode.crop_left = (kFullWidth - kCenterCropSize) / 2;
    mode.crop_top = (kFullHeight - kCenterCropSize) / 2;
    mode.crop_right = mode.crop_left;
    mode.crop_bottom = mode.crop_top;
    mode.output_name = "center_crop";
    return mode;
}

CaptureMode MakeHorizontalFlipMode() {
    CaptureMode mode;
    mode.description = "Horizontal flip";
    mode.horizontal_flip = 1;
    mode.output_name = "flip_horizontal";
    return mode;
}

CaptureMode MakeVerticalFlipMode() {
    CaptureMode mode;
    mode.description = "Vertical flip";
    mode.vertical_flip = 1;
    mode.output_name = "flip_vertical";
    return mode;
}

const std::map<std::string, CaptureMode> kModes = {
    {"full", CaptureMode{}},
    {"subsample-half", MakeSubsampleHalfMode()},
    {"center-crop", MakeCenterCropMode()},
    {"flip-horizontal", MakeHorizontalFlipMode()},
    {"flip-vertical", MakeVerticalFlipMode()},
};

void PrintUsage(const char* program) {
    std::cerr << "Usage: " << program << " /dev/videoN [mode]\n\nModes:\n";
    for (const auto& [name, mode] : kModes) {
        std::cerr << "  " << name << "\t" << mode.description << '\n';
    }
}

std::string MakePipeline(const CaptureMode& mode) {
    return "v4l2src name=camera io-mode=mmap "
           "extra-controls=\"c," +
           std::string(kImx678Controls) +
           ","
           "image_subsampling=" +
           std::to_string(mode.subsampling) +
           ",crop_left=" + std::to_string(mode.crop_left) +
           ",crop_top=" + std::to_string(mode.crop_top) +
           ",crop_right=" + std::to_string(mode.crop_right) +
           ",crop_bottom=" + std::to_string(mode.crop_bottom) +
           ",horizontal_flip=" + std::to_string(mode.horizontal_flip) +
           ",vertical_flip=" + std::to_string(mode.vertical_flip) + "\" ! " +
           "video/x-raw,format=YUY2,width=" + std::to_string(mode.output_width) +
           ",height=" + std::to_string(mode.output_height) + ",framerate=30/1 ! " +
           "videoconvert ! video/x-raw,format=RGB ! "
           "pngenc snapshot=false ! "
           "multifilesink index=1 location=outputs/" +
           std::string(mode.output_name) + "_%02d.png";
}

std::filesystem::path MakeOutputPath(const CaptureMode& mode, int frame_number) {
    std::ostringstream file_name;
    file_name << mode.output_name << '_' << std::setfill('0') << std::setw(2)
              << frame_number << ".png";
    return std::filesystem::path("outputs") / file_name.str();
}

bool RemovePreviousOutputs(const CaptureMode& mode) {
    for (int frame_number = 1; frame_number <= kFrameCount; ++frame_number) {
        const auto output_path = MakeOutputPath(mode, frame_number);
        std::error_code error;
        std::filesystem::remove(output_path, error);
        if (error) {
            std::cerr << "Could not remove " << output_path << ": " << error.message()
                      << '\n';
            return false;
        }
    }
    return true;
}

bool ValidateOutputs(const CaptureMode& mode) {
    for (int frame_number = 1; frame_number <= kFrameCount; ++frame_number) {
        const auto output_path = MakeOutputPath(mode, frame_number);
        std::error_code error;
        const auto file_size = std::filesystem::file_size(output_path, error);
        if (error || file_size == 0) {
            std::cerr << "Output file is missing or empty: " << output_path << '\n';
            return false;
        }
    }
    return true;
}

void PrintGstreamerError(GstMessage* message, const char* fallback_message) {
    if (message == nullptr) {
        std::cerr << fallback_message << '\n';
        return;
    }

    GError* error = nullptr;
    gchar* debug = nullptr;
    gst_message_parse_error(message, &error, &debug);
    std::cerr << "GStreamer error: "
              << (error != nullptr ? error->message : fallback_message) << '\n';
    if (debug != nullptr) {
        std::cerr << "Debug: " << debug << '\n';
    }
    g_free(debug);
    g_clear_error(&error);
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 2 || argc > 3) {
        PrintUsage(argv[0]);
        return 1;
    }

    const std::string mode_name = argc == 3 ? argv[2] : "full";
    const auto mode_entry = kModes.find(mode_name);
    if (mode_entry == kModes.end()) {
        std::cerr << "Unknown mode: " << mode_name << "\n\n";
        PrintUsage(argv[0]);
        return 1;
    }
    const CaptureMode& mode = mode_entry->second;

    gst_init(nullptr, nullptr);
    std::error_code directory_error;
    std::filesystem::create_directories("outputs", directory_error);
    if (directory_error) {
        std::cerr << "Could not create the outputs directory: "
                  << directory_error.message() << '\n';
        return 1;
    }
    if (!RemovePreviousOutputs(mode)) {
        return 1;
    }

    GError* error = nullptr;
    const std::string pipeline_text = MakePipeline(mode);
    GstElement* pipeline = gst_parse_launch(pipeline_text.c_str(), &error);
    if (pipeline == nullptr || error != nullptr) {
        std::cerr << "Pipeline error: "
                  << (error != nullptr ? error->message : "unknown error") << '\n';
        g_clear_error(&error);
        if (pipeline != nullptr) {
            gst_object_unref(pipeline);
        }
        return 1;
    }

    GstElement* camera = gst_bin_get_by_name(GST_BIN(pipeline), "camera");
    if (camera == nullptr) {
        std::cerr << "The camera element was not created.\n";
        gst_object_unref(pipeline);
        return 1;
    }

    g_object_set(camera, "device", argv[1], "num-buffers", kFrameCount, nullptr);
    gst_object_unref(camera);

    std::cout << "Mode: " << mode.description << '\n';
    std::cout << "Capture " << kFrameCount << " frames from " << argv[1] << ".\n";

    GstBus* bus = gst_element_get_bus(pipeline);
    if (gst_element_set_state(pipeline, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
        GstMessage* message = gst_bus_timed_pop_filtered(
            bus, GST_SECOND, static_cast<GstMessageType>(GST_MESSAGE_ERROR));
        PrintGstreamerError(message, "The pipeline could not start.");
        if (message != nullptr) {
            gst_message_unref(message);
        }
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(bus);
        gst_object_unref(pipeline);
        return 1;
    }

    GstMessage* message = gst_bus_timed_pop_filtered(
        bus, kCaptureTimeoutSeconds * GST_SECOND,
        static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    int exit_code = 0;
    if (message == nullptr) {
        std::cerr << "Capture timed out.\n";
        exit_code = 1;
    } else if (GST_MESSAGE_TYPE(message) == GST_MESSAGE_ERROR) {
        PrintGstreamerError(message, "Capture failed.");
        exit_code = 1;
    }

    if (message != nullptr) {
        gst_message_unref(message);
    }
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(bus);
    gst_object_unref(pipeline);

    if (exit_code == 0 && !ValidateOutputs(mode)) {
        exit_code = 1;
    }
    if (exit_code == 0) {
        std::cout << "Done: outputs/" << mode.output_name << "_01.png through "
                  << mode.output_name << '_' << std::setfill('0') << std::setw(2)
                  << kFrameCount << ".png\n";
    }
    return exit_code;
}
