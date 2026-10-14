#include <gst/gst.h>

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iostream>
#include <string>
#include <system_error>

namespace {
constexpr int kPipelineTimeoutSeconds = 30;
constexpr char kVideoPrefix[] = "/dev/video";

bool MakeConverterName(const char* path, std::string& name) {
    const std::string device = path;
    const std::size_t prefix_length = sizeof(kVideoPrefix) - 1;
    if (device.compare(0, prefix_length, kVideoPrefix) != 0 ||
        device.size() == prefix_length ||
        !std::all_of(device.begin() + prefix_length, device.end(),
                     [](unsigned char c) { return std::isdigit(c) != 0; })) {
        return false;
    }
    name = "v4l2video" + device.substr(prefix_length) + "convert";
    return true;
}
}  // namespace

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " INPUT.png /dev/videoN\n";
        return 1;
    }

    std::string converter_name;
    if (!MakeConverterName(argv[2], converter_name)) {
        std::cerr << "Invalid video device: " << argv[2]
                  << ". Use /dev/video followed by one or more digits.\n";
        return 1;
    }

    gst_init(nullptr, nullptr);
    std::error_code directory_error;
    std::filesystem::create_directories("outputs", directory_error);
    if (directory_error) {
        std::cerr << "Could not create the outputs directory: "
                  << directory_error.message() << '\n';
        return 1;
    }

    GError* error = nullptr;
    // The video node number selects the matching MAPI converter.
    const std::string pipeline_description =
        "filesrc name=input ! pngdec ! "
        "video/x-raw,format=RGB,width=640,height=640 ! " +
        converter_name +
        " output-io-mode=mmap capture-io-mode=mmap "
        "extra-controls=\"c,scaler_coefficient_mode_and_sel=1\" ! "
        "video/x-raw,format=RGB,width=1024,height=1024 ! "
        "pngenc ! filesink location=outputs/result_mapi_scale.png";

    GstElement* pipeline = gst_parse_launch(pipeline_description.c_str(), &error);

    if (pipeline == nullptr || error != nullptr) {
        std::cerr << "GStreamer error: "
                  << (error != nullptr ? error->message : "pipeline creation failed")
                  << '\n';
        g_clear_error(&error);
        if (pipeline != nullptr) gst_object_unref(pipeline);
        return 1;
    }

    GstElement* input = gst_bin_get_by_name(GST_BIN(pipeline), "input");
    if (input == nullptr) {
        std::cerr << "The input element was not created.\n";
        gst_object_unref(pipeline);
        return 1;
    }

    g_object_set(input, "location", argv[1], nullptr);
    gst_object_unref(input);

    std::cout << "Scale with MAPI: 640x640 -> 1024x1024.\n";

    if (gst_element_set_state(pipeline, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
        std::cerr << "The pipeline could not start.\n";
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);
        return 1;
    }

    GstBus* bus = gst_element_get_bus(pipeline);

    GstMessage* message = gst_bus_timed_pop_filtered(
        bus, kPipelineTimeoutSeconds * GST_SECOND,
        static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
    bool success = false;
    if (message == nullptr) {
        std::cerr << "The pipeline timed out.\n";
    } else if (GST_MESSAGE_TYPE(message) == GST_MESSAGE_ERROR) {
        gst_message_parse_error(message, &error, nullptr);
        std::cerr << "GStreamer error: " << error->message << '\n';
        g_clear_error(&error);
    } else {
        success = true;
    }
    gst_element_set_state(pipeline, GST_STATE_NULL);
    if (message != nullptr) gst_message_unref(message);
    gst_object_unref(bus);
    gst_object_unref(pipeline);

    if (!success) {
        return 1;
    }

    std::cout << "Done: outputs/result_mapi_scale.png\n";
    return 0;
}
