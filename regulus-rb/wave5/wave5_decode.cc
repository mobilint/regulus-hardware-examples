#include <gst/gst.h>

#include <filesystem>
#include <iostream>
#include <string>

constexpr GstClockTime kPipelineTimeout = 90 * GST_SECOND;

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " INPUT.mp4\n";
        return 1;
    }

    gst_init(nullptr, nullptr);
    std::error_code directory_error;
    std::filesystem::create_directories("outputs", directory_error);
    if (directory_error) {
        std::cerr << "Failed to create output directory: " << directory_error.message()
                  << '\n';
        return 1;
    }

    const std::filesystem::path input_path = argv[1];
    const std::string output_path = "outputs/" + input_path.stem().string() + ".nv12";
    const std::string pipeline_description =
        "filesrc name=input ! qtdemux ! h264parse ! "
        "v4l2h264dec output-io-mode=mmap capture-io-mode=mmap ! "
        "video/x-raw,format=NV12,width=640,height=640 ! "
        "filesink name=output";

    GError* error = nullptr;
    GstElement* pipeline = gst_parse_launch(pipeline_description.c_str(), &error);
    if (pipeline == nullptr || error != nullptr) {
        if (error != nullptr) {
            std::cerr << "GStreamer error: " << error->message << '\n';
            g_clear_error(&error);
        } else {
            std::cerr << "Failed to create the GStreamer pipeline.\n";
        }
        if (pipeline != nullptr) {
            gst_object_unref(pipeline);
        }
        return 1;
    }

    GstElement* input = gst_bin_get_by_name(GST_BIN(pipeline), "input");
    GstElement* output = gst_bin_get_by_name(GST_BIN(pipeline), "output");
    if (input == nullptr || output == nullptr) {
        std::cerr << "Failed to find the input or output element.\n";
        if (input != nullptr) {
            gst_object_unref(input);
        }
        if (output != nullptr) {
            gst_object_unref(output);
        }
        gst_object_unref(pipeline);
        return 1;
    }
    g_object_set(input, "location", argv[1], nullptr);
    g_object_set(output, "location", output_path.c_str(), nullptr);
    gst_object_unref(input);
    gst_object_unref(output);

    std::cout << "Decode with Wave5: H.264 -> NV12.\n";
    if (gst_element_set_state(pipeline, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
        std::cerr << "Failed to start the GStreamer pipeline.\n";
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);
        return 1;
    }

    GstBus* bus = gst_element_get_bus(pipeline);
    GstMessage* message = gst_bus_timed_pop_filtered(
        bus, kPipelineTimeout,
        static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
    const bool success =
        message != nullptr && GST_MESSAGE_TYPE(message) == GST_MESSAGE_EOS;

    if (message == nullptr) {
        std::cerr << "GStreamer pipeline timed out.\n";
    } else if (!success) {
        gst_message_parse_error(message, &error, nullptr);
        std::cerr << "GStreamer error: " << error->message << '\n';
        g_clear_error(&error);
    }

    gst_element_set_state(pipeline, GST_STATE_NULL);
    if (message != nullptr) {
        gst_message_unref(message);
    }
    gst_object_unref(bus);
    gst_object_unref(pipeline);

    if (!success) {
        return 1;
    }

    std::cout << "Done: " << output_path << '\n';
    return 0;
}
