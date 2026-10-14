#include <gst/app/gstappsink.h>
#include <gst/gst.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " INPUT.jpg\n";
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
    // CODAJ produces one 640x640 NV12 frame for the application.
    const std::string pipeline_description =
        "filesrc name=input ! jpegparse ! "
        "v4l2jpegdec output-io-mode=mmap capture-io-mode=mmap ! "
        "video/x-raw,format=NV12,width=640,height=640 ! "
        "appsink name=output sync=false max-buffers=1";

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
    gst_object_unref(input);

    std::cout << "Decode with CODAJ: JPEG -> NV12.\n";
    if (gst_element_set_state(pipeline, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
        std::cerr << "Failed to start the GStreamer pipeline.\n";
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(output);
        gst_object_unref(pipeline);
        return 1;
    }

    GstSample* sample =
        gst_app_sink_try_pull_sample(GST_APP_SINK(output), 5 * GST_SECOND);
    if (sample == nullptr) {
        GstBus* bus = gst_element_get_bus(pipeline);
        GstMessage* message = gst_bus_pop_filtered(bus, GST_MESSAGE_ERROR);
        if (message != nullptr) {
            gst_message_parse_error(message, &error, nullptr);
            std::cerr << "GStreamer error: " << error->message << '\n';
            g_clear_error(&error);
            gst_message_unref(message);
        } else {
            std::cerr << "JPEG decoding timed out.\n";
        }
        gst_object_unref(bus);
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(output);
        gst_object_unref(pipeline);
        return 1;
    }

    GstBuffer* buffer = gst_sample_get_buffer(sample);
    GstMapInfo data;
    if (!gst_buffer_map(buffer, &data, GST_MAP_READ)) {
        std::cerr << "Failed to read the decoded frame.\n";
        gst_sample_unref(sample);
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(output);
        gst_object_unref(pipeline);
        return 1;
    }

    std::ofstream output_file(output_path, std::ios::binary);
    if (!output_file.is_open()) {
        std::cerr << "Failed to open output file: " << output_path << '\n';
        gst_buffer_unmap(buffer, &data);
        gst_sample_unref(sample);
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(output);
        gst_object_unref(pipeline);
        return 1;
    }

    output_file.write(reinterpret_cast<const char*>(data.data), data.size);
    output_file.close();
    if (!output_file) {
        std::cerr << "Failed to write output file: " << output_path << '\n';
        gst_buffer_unmap(buffer, &data);
        gst_sample_unref(sample);
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(output);
        gst_object_unref(pipeline);
        return 1;
    }

    gst_buffer_unmap(buffer, &data);
    gst_sample_unref(sample);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(output);
    gst_object_unref(pipeline);

    std::cout << "Done: " << output_path << '\n';
    return 0;
}
