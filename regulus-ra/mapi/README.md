# Regulus-RA MAPI Examples

[Korean](README(KR).md)

These C++ examples use the Regulus-RA MAPI hardware to change image sizes and pixel formats.

## What Is MAPI?

MAPI is hardware that converts uncompressed image pixel data.

MAPI changes image sizes and converts RGB/YUV data in hardware instead of on the CPU.

GStreamer decompresses the input PNG file and compresses the output PNG file.

## Examples

### `mapi_scale`

This example uses MAPI to scale a 640x640 PNG image to 1024x1024.

```text
test-sample.png
    -> GStreamer PNG decoder
    -> RGB 640x640
    -> MAPI scaler
    -> RGB 1024x1024
    -> GStreamer PNG encoder
    -> outputs/result_mapi_scale.png
```

### `mapi_csc`

This example uses MAPI to convert 640x640 NV12 pixel data to RGB data.

```text
test-sample2.nv12
    -> GStreamer raw video parser
    -> NV12 640x640
    -> MAPI CSC
    -> RGB 640x640
    -> GStreamer PNG encoder
    -> outputs/result_mapi_csc.png
```

An NV12 file does not contain resolution or pixel format information. This example interprets the input as 640x640 NV12 data.

The NV12 input file size is `640 x 640 x 1.5 = 614400 bytes`.

## Files

```text
mapi/
├── Makefile
├── README.md
├── README(KR).md
├── mapi_csc.cc
├── mapi_scale.cc
├── test-sample.png
└── test-sample2.nv12
```

## Build

The Makefile automatically selects the latest valid cross toolchain in `/opt/crosstools/mobilint/1.0.0`.

Run these commands from the repository root directory:

```bash
cd regulus-ra/mapi
make
```

To use a specific version, set `TOOLCHAIN_ROOT`.
Replace `X.Y.Z` with an installed cross-toolchain version.

```bash
make clean
make TOOLCHAIN_ROOT=/opt/crosstools/mobilint/1.0.0/vX.Y.Z
```

## Transfer to the Board

Go to the `regulus-ra` directory from the repository root directory.

```bash
cd regulus-ra
scp -r mapi root@<BOARD_IP>:/root/
```

## Check the MAPI Device

Use this command on the board to show the V4L2 devices:

```bash
v4l2-ctl --list-devices
```

Find `C&M MAPI` and the `/dev/videoN` node below it.

Use this command to show the MAPI video node that GStreamer selected:

```bash
gst-inspect-1.0 v4l2convert | grep 'Default: "/dev/video'
```

The Regulus-RA `v4l2convert` element automatically finds the MAPI video node. You do not specify a video node when you run these examples.

## Run

```bash
cd /root/mapi
./mapi_scale test-sample.png
./mapi_csc test-sample2.nv12
```

Inspect the output files:

```bash
file outputs/result_mapi_scale.png
file outputs/result_mapi_csc.png
```

## Copy the Results to the Development PC

Run this command from the repository root directory on the development PC:

```bash
scp -r root@<BOARD_IP>:/root/mapi/outputs regulus-ra/mapi/
```
