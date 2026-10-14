#include <sp500/letterbox.h>

#include <cstdio>
#include <filesystem>
#include <opencv2/imgcodecs.hpp>
#include <system_error>

int main(int argc, char** argv) {
    if (argc < 2 || argc > 3) {
        std::fprintf(stderr, "usage: %s <input image> [output image]\n", argv[0]);
        return 1;
    }

    const bool use_default_output = argc == 2;
    const char* output_path = use_default_output ? "outputs/letterbox_out.png" : argv[2];
    if (use_default_output) {
        std::error_code error;
        std::filesystem::create_directories("outputs", error);
        if (error) {
            std::fprintf(stderr, "cannot create outputs directory: %s\n",
                         error.message().c_str());
            return 1;
        }
    }

    cv::Mat input = cv::imread(argv[1], cv::IMREAD_COLOR);
    if (input.empty()) {
        std::fprintf(stderr, "cannot read %s\n", argv[1]);
        return 1;
    }

    sp500_status status = sp500_dsp_open(0);
    if (status != SP500_OK) {
        std::fprintf(stderr, "sp500_dsp_open: %s\n", sp500_status_string(status));
        return 1;
    }

    sp500_letterbox_params params;
    sp500_letterbox_params_init(&params);
    params.src_w = input.cols;
    params.src_h = input.rows;

    sp500_buffer src = sp500_buffer_wrap(input.data, input.total() * input.elemSize());
    const uint32_t output_size = params.dst_w * params.dst_h * params.channels;
    sp500_buffer dst = sp500_buffer_alloc(output_size, SP500_ALIGN_NPU);
    if (dst.data == nullptr) {
        std::fprintf(stderr, "sp500_buffer_alloc failed\n");
        return 1;
    }

    sp500_letterbox_result result;
    status = sp500_letterbox(&src, &params, &dst, &result);
    if (status != SP500_OK) {
        std::fprintf(stderr, "sp500_letterbox: %s\n", sp500_status_string(status));
        sp500_buffer_release(&dst);
        return 1;
    }

    cv::Mat output(params.dst_h, params.dst_w, CV_8UC3, dst.data);
    const bool saved = cv::imwrite(output_path, output);
    sp500_buffer_release(&dst);
    if (!saved) {
        std::fprintf(stderr, "cannot write %s\n", output_path);
        return 1;
    }

    std::printf("saved %s (%dx%d, %.3f ms)\n", output_path, params.dst_w, params.dst_h,
                result.host_seconds * 1000.0);
    return 0;
}
