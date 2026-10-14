# Regulus Hardware Examples

[Korean](README(KR).md)

<!-- markdownlint-disable MD033 -->
<div align="center">
<p>
<a href="https://www.mobilint.com/" target="_blank">
<img src="./assets/Mobilint_Logo.png" alt="Mobilint Logo" width="60%">
</a>
</p>
</div>
<!-- markdownlint-enable MD033 -->

This repository provides C++ examples for the hardware in the Mobilint Regulus SoCs.

Each example processes image or video data in hardware. It saves the result to a file for inspection.

## Before You Start

You need a Regulus board, its SDK, and its cross toolchain.

Download the SDK from the [Mobilint Download Center](https://dl.mobilint.com/).

For product-specific setup, build, transfer, and run instructions, refer to these documents:

- [Regulus-RA Hardware Examples](regulus-ra/)
- [Regulus-RB Hardware Examples](regulus-rb/)

## Supported Boards

This repository supports these Regulus boards:

- **Regulus-RA**
- **Regulus-RB**

The available hardware and cross toolchain are different for each board. Use the directory for your board.

## Overview

| Hardware | Function | Regulus-RA | Regulus-RB |
| --- | --- | :---: | :---: |
| MAPI | Changes the image size and converts RGB/YUV pixel formats. | [Example](regulus-ra/mapi/) | [Example](regulus-rb/mapi/) |
| CODAJ | Encodes or decodes JPEG images. | [Example](regulus-ra/codaj/) | [Example](regulus-rb/codaj/) |
| DMA350 | Copies memory and crops, rotates, or mirrors images. | - | [Example](regulus-rb/dma350/) |
| ISP | Processes and captures RAW images from a MIPI camera. | [Example](regulus-ra/isp/) | [Example](regulus-rb/isp/) |
| Wave5 | Encodes or decodes H.264/H.265 video. | [Example](regulus-ra/wave5/) | [Example](regulus-rb/wave5/) |
| SP500 | Runs image and signal processing operations on a Vision DSP. | - | [Example](regulus-rb/sp500/) |

A `-` means that this repository does not provide an example for that board.

The `README.md` in each directory explains the hardware function, data flow, build procedure, and run procedure.

## Procedure

1. Go to the directory for your board.
2. Install the cross toolchain for the board.
3. Run `make` in the hardware example directory.
4. Transfer the example directory to the board.
5. Run the example on the board and inspect the files in `outputs/`.

Refer to the `README.md` in each example directory for detailed commands.

## Repository Structure

```text
regulus-hardware-examples-gitlab/
├── assets/             # Images for the documentation
├── regulus-ra/         # Regulus-RA hardware examples
│   ├── codaj/          # JPEG encoding and decoding
│   ├── isp/            # MIPI camera ISP capture
│   ├── mapi/           # Image size and pixel format conversion
│   └── wave5/          # H.264/H.265 encoding and decoding
└── regulus-rb/         # Regulus-RB hardware examples
    ├── codaj/          # JPEG encoding and decoding
    ├── dma350/         # Memory copy and image layout conversion
    ├── isp/            # MIPI camera ISP capture
    ├── mapi/           # Image size and pixel format conversion
    ├── sp500/          # SP500 letterbox preprocessing
    └── wave5/          # H.264/H.265 encoding and decoding
```

## Support and Issues

If you have a problem with an example, contact [Mobilint technical support](mailto:tech-support@mobilint.com).
