#include <gst/gst.h>

#include <filesystem>
#include <iostream>

constexpr GstClockTime kPipelineTimeout = 90 * GST_SECOND;

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " INPUT.png\n";
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

    const char* pipeline_description =
        "filesrc name=input ! pngdec ! "
        "video/x-raw,format=RGB,width=640,height=640 ! "
        "v4l2convert output-io-mode=mmap capture-io-mode=mmap "
        "extra-controls=\"c,scaler_coefficient_mode_and_sel=1\" ! "
        "video/x-raw,format=RGB,width=1024,height=1024 ! "
        "pngenc ! filesink location=outputs/result_mapi_scale.png";

    GError* error = nullptr;
    GstElement* pipeline = gst_parse_launch(pipeline_description, &error);
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
    if (input == nullptr) {
        std::cerr << "Failed to find the input element.\n";
        gst_object_unref(pipeline);
        return 1;
    }
    g_object_set(input, "location", argv[1], nullptr);
    gst_object_unref(input);

    std::cout << "Scale with MAPI: 640x640 -> 1024x1024.\n";
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

    std::cout << "Done: outputs/result_mapi_scale.png\n";
    return 0;
}
