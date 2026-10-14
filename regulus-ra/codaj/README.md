# Regulus-RA CODAJ Examples

[Korean](README(KR).md)

These C++ examples use the Regulus-RA CODAJ hardware to encode or decode JPEG images.

## What Is CODAJ?

CODAJ is a hardware codec for JPEG and MJPEG data.

- The JPEG encoder compresses NV12 pixel data into JPEG data.
- The JPEG decoder decompresses JPEG data into NV12 pixel data.

## Examples

### `codaj_decode`

```text
test-sample.jpg
    -> CODAJ JPEG decoder
    -> NV12 640x640
    -> outputs/test-sample.nv12
```

### `codaj_encode`

```text
test-sample2.nv12
    -> NV12 640x640
    -> CODAJ JPEG encoder
    -> outputs/test-sample2.jpeg
```

An NV12 file does not contain resolution or pixel format information. This example interprets the input as 640x640 NV12 data.

The NV12 file size is `640 x 640 x 1.5 = 614400 bytes`.

## Files

```text
codaj/
├── Makefile
├── README.md
├── README(KR).md
├── codaj_decode.cc
├── codaj_encode.cc
├── test-sample.jpg
└── test-sample2.nv12
```

## Build

The Makefile automatically selects the latest valid cross toolchain in `/opt/crosstools/mobilint/1.0.0`.

Run these commands from the repository root directory:

```bash
cd regulus-ra/codaj
make
```

To use a specific version, set `TOOLCHAIN_ROOT`.
Replace `X.Y.Z` with an installed cross-toolchain version.

```bash
make clean
make TOOLCHAIN_ROOT=/opt/crosstools/mobilint/1.0.0/vX.Y.Z
```

## Transfer to the Board

```bash
cd regulus-ra
scp -r codaj root@<BOARD_IP>:/root/
```

## Check the CODAJ Device

Use this command on the board to show the V4L2 devices:

```bash
v4l2-ctl --list-devices
```

Find `C&M CODAJ Encoder` and the encoder and decoder video nodes below it.

Use these commands to show the video nodes that GStreamer selected:

```bash
gst-inspect-1.0 v4l2jpegenc | grep 'Default: "/dev/video'
gst-inspect-1.0 v4l2jpegdec | grep 'Default: "/dev/video'
```

The `v4l2jpegenc` and `v4l2jpegdec` elements automatically find the CODAJ video nodes. You do not specify a video node when you run these examples.

## Run

```bash
cd /root/codaj
./codaj_decode test-sample.jpg
./codaj_encode test-sample2.nv12
```

Inspect the output files:

```bash
stat -c '%n: %s bytes' outputs/test-sample.nv12
file outputs/test-sample2.jpeg
```

## Copy the Results to the Development PC

```bash
scp -r root@<BOARD_IP>:/root/codaj/outputs regulus-ra/codaj/
```
