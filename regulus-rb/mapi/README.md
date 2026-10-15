# Regulus-RB MAPI Examples

[Korean](README(KR).md)

These C++ examples use the Regulus-RB MAPI hardware to change image sizes and pixel formats.

## What Is MAPI?

MAPI is hardware that converts uncompressed image pixel data.
MAPI changes image sizes and converts RGB/YUV data in hardware instead of on the CPU.

The MAPI driver provides these primary functions:

- Image scaling
- Color space conversion (CSC) between RGB and YUV
- RGB/YUV pixel format conversion
- YUV 4:4:4, 4:2:2, and 4:2:0 conversion
- Bilinear, Cubic, and Lanczos-2 scale filters
- User CSC coefficients and luma/chroma phase settings

PNG and JPEG are compressed image file formats.
MAPI does not directly read or save these files. It processes uncompressed pixel data, such as RGB24, NV12, and YUYV data.
In these examples, GStreamer decompresses and compresses the PNG data.

## Examples

### `mapi_scale`

This example reads a 640x640 PNG image, scales it to 1024x1024 in MAPI, and saves it as a PNG file.

```text
test-sample.png
    -> GStreamer PNG decoder
    -> RGB 640x640
    -> MAPI scaler
    -> RGB 1024x1024
    -> GStreamer PNG encoder
    -> outputs/result_mapi_scale.png
```

The MAPI scale filter uses Cubic sharpness 1.0.

### `mapi_csc`

This example reads uncompressed 640x640 NV12 pixel data, converts it to RGB data in MAPI, and saves it as a PNG file.

```text
test-sample2.nv12
    -> GStreamer rawvideoparse
    -> NV12 640x640
    -> MAPI CSC
    -> RGB 640x640
    -> GStreamer PNG encoder
    -> outputs/result_mapi_csc.png
```

An NV12 file does not contain resolution or pixel format information.
This example interprets the input as 640x640 NV12 data.
The uncompressed pixel data size is:

```text
640 x 640 x 1.5 = 614400 bytes
```

## Files

```text
mapi/
├── Makefile
├── README.md
├── README(KR).md
├── mapi_scale.cc
├── mapi_csc.cc
├── test-sample.png
└── test-sample2.nv12
```

The build creates the `mapi_scale` and `mapi_csc` executables.
The programs create the `outputs` directory and the output images.

## Build

The Makefile automatically selects the latest valid cross toolchain in `/opt/crosstools/mobilint/2.0.0`.

Go to the `mapi` directory from the repository root directory. Then, build the examples.

```bash
cd regulus-rb/mapi
make
```

To use a specific version, set `TOOLCHAIN_ROOT`.
Replace `X.Y.Z` with an installed cross-toolchain version.

```bash
make clean
make TOOLCHAIN_ROOT=/opt/crosstools/mobilint/2.0.0/vX.Y.Z
```

The build creates these executables:

```text
mapi_scale
mapi_csc
```

## Transfer to the Board

Go to the `regulus-rb` directory from the repository root directory.

```bash
cd regulus-rb
```

Transfer the complete `mapi` directory. The directory contains the executables and test inputs.

```bash
scp -r mapi root@<BOARD_IP>:/root/
```

The directory on the board is:

```text
/root/mapi/
```

Replace `<BOARD_IP>` with the IP address of the Regulus-RB board.

## Check the MAPI Device

Connect to the board.

```bash
ssh root@<BOARD_IP>
```

Show the registered V4L2 devices and video nodes.

```bash
v4l2-ctl --list-devices
```

The MAPI video node can be different for each product configuration.
Find `C&M MAPI` and the `/dev/videoN` node below it.

```text
C&M MAPI (platform:regulus-mapi):
    /dev/videoN
    /dev/mediaN
```

For example, one product can use `/dev/video0`, and another product can use `/dev/video1`.
Use the MAPI video node from this output in the run commands.

## Run

Go to the transferred directory on the board.

```bash
cd /root/mapi
```

Run the image scaling example.
Use the MAPI video node as the last argument.

```bash
./mapi_scale test-sample.png /dev/videoN
```

Run the CSC example.

```bash
./mapi_csc test-sample2.nv12 /dev/videoN
```

If MAPI is `/dev/video0`, use these commands:

```bash
./mapi_scale test-sample.png /dev/video0
./mapi_csc test-sample2.nv12 /dev/video0
```

The program selects the V4L2 converter that is connected to the specified `/dev/videoN` node.
It checks for a device-specific converter first.
If the node uses the generic `v4l2convert` element, the program verifies the element's `device` property.
The program does not verify that the selected node is a MAPI device.
Always use the node below `C&M MAPI` in the `v4l2-ctl --list-devices` output.

Inspect the output files:

```bash
file outputs/result_mapi_scale.png
file outputs/result_mapi_csc.png
```

The expected results are:

| Output file | Size | Pixel format |
| --- | ---: | --- |
| `outputs/result_mapi_scale.png` | 1024x1024 | RGB |
| `outputs/result_mapi_csc.png` | 640x640 | RGB |

## Copy the Results to the Development PC

On the development PC, go to the `regulus-rb` directory. Then, copy the `outputs` directory from the board.

```bash
cd regulus-rb
scp -r root@<BOARD_IP>:/root/mapi/outputs mapi/
```

The results are in this directory on the development PC:

```text
regulus-rb/mapi/outputs/
```
