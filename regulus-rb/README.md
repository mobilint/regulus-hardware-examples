# Regulus-RB Hardware Examples

[Korean](README(KR).md)

These C++ examples use the image, video, and data processing hardware in Regulus-RB.

## Hardware

| Component | Description |
| --- | --- |
| [MAPI](mapi/) | Changes the image size and converts RGB/YUV pixel formats. |
| [CODAJ](codaj/) | Encodes or decodes JPEG images. |
| [DMA350](dma350/) | Copies memory and crops, rotates, or mirrors images. |
| [ISP](isp/) | Processes and captures RAW images from a MIPI camera. |
| [SP500](sp500/) | Runs image and signal processing operations on a Vision DSP. |
| [Wave5](wave5/) | Encodes or decodes H.264/H.265 video. |

Refer to the `README.md` in each directory for build and run instructions.

## Build Preparation

Install the Regulus-RB SDK and cross toolchain on the development PC before you build an example.

Download the SDK from [MOBILINT Download](https://dl.mobilint.com).

The Makefile in each example automatically selects the latest valid cross toolchain in `/opt/crosstools/mobilint/2.0.0`.
You do not have to run the environment setup script.

To use a specific version, set `TOOLCHAIN_ROOT` in the example directory.
Replace `X.Y.Z` with an installed cross-toolchain version.

```bash
make clean
make TOOLCHAIN_ROOT=/opt/crosstools/mobilint/2.0.0/vX.Y.Z
```

## Hardware Check

Use this command to show the V4L2 hardware on the board:

```bash
v4l2-ctl --list-devices
```

The number in `/dev/videoN` can be different for each product configuration.
