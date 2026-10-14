# Regulus-RB CODAJ Examples

[Korean](README(KR).md)

These C++ examples use the Regulus-RB CODAJ hardware to encode or decode JPEG images.

## What Is CODAJ?

CODAJ is a hardware codec for JPEG and MJPEG data.
CODAJ processes JPEG encoding and decoding in hardware instead of on the CPU.

- The JPEG encoder compresses uncompressed pixel data, such as NV12 data, into JPEG data.
- The JPEG decoder decompresses JPEG data into uncompressed pixel data, such as NV12 data.

## Examples

### `codaj_decode`

This example decodes a 640x640 JPEG image into an NV12 file.

```text
test-sample.jpg
    -> GStreamer JPEG parser
    -> CODAJ JPEG decoder
    -> NV12 640x640
    -> outputs/test-sample.nv12
```

### `codaj_encode`

This example encodes a 640x640 NV12 file into a JPEG image.

```text
test-sample2.nv12
    -> GStreamer raw video parser
    -> NV12 640x640
    -> CODAJ JPEG encoder
    -> outputs/test-sample2.jpeg
```

An NV12 file does not contain resolution or pixel format information.
This example interprets the input as 640x640 NV12 data.
The file size is:

```text
640 x 640 x 1.5 = 614400 bytes
```

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

The build creates the `codaj_decode` and `codaj_encode` executables.
The programs create the `outputs` directory and the output files.

## Build

The Makefile automatically selects the latest valid cross toolchain in `/opt/crosstools/mobilint/2.0.0`.

Go to the `codaj` directory from the repository root directory. Then, build the examples.

```bash
cd regulus-rb/codaj
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
codaj_decode
codaj_encode
```

## Transfer to the Board

Go to the `regulus-rb` directory from the repository root directory.

```bash
cd regulus-rb
```

Transfer the complete `codaj` directory. The directory contains the executables and test inputs.

```bash
scp -r codaj root@<BOARD_IP>:/root/
```

The directory on the board is:

```text
/root/codaj/
```

Replace `<BOARD_IP>` with the IP address of the Regulus-RB board.

## Check the CODAJ Device

Connect to the board.

```bash
ssh root@<BOARD_IP>
```

Show the registered V4L2 devices and video nodes.

```bash
v4l2-ctl --list-devices
```

Find `C&M CODAJ` and the two `/dev/videoN` nodes below it.
One video node is the JPEG encoder. The other video node is the JPEG decoder.
The video node numbers can be different for each product configuration.

Use these commands to show the encoder and decoder video nodes that GStreamer selected:

```bash
gst-inspect-1.0 v4l2jpegenc | grep 'Default: "/dev/video'
gst-inspect-1.0 v4l2jpegdec | grep 'Default: "/dev/video'
```

The `v4l2jpegenc` and `v4l2jpegdec` elements automatically find the CODAJ video nodes.
You do not specify a `/dev/videoN` node when you run these examples.

## Run

Go to the transferred directory on the board.

```bash
cd /root/codaj
```

Run the JPEG decoding example.

```bash
./codaj_decode test-sample.jpg
```

Run the NV12 encoding example.

```bash
./codaj_encode test-sample2.nv12
```

Inspect the output files.

```bash
stat -c '%n: %s bytes' outputs/test-sample.nv12
file outputs/test-sample2.jpeg
```

The expected results are:

| Output file | Size or format |
| --- | --- |
| `outputs/test-sample.nv12` | 614400 bytes, NV12 640x640 |
| `outputs/test-sample2.jpeg` | JPEG image data, 640x640 |

## Copy the Results to the Development PC

On the development PC, go to the `regulus-rb` directory. Then, copy the `outputs` directory from the board.

```bash
cd regulus-rb
scp -r root@<BOARD_IP>:/root/codaj/outputs codaj/
```

The results are in this directory on the development PC:

```text
regulus-rb/codaj/outputs/
```

You can open the JPEG result directly in the IDE.
To inspect the NV12 result, specify its resolution and pixel format and convert it to PNG.

```bash
ffmpeg -f rawvideo -pixel_format nv12 -video_size 640x640 \
    -i codaj/outputs/test-sample.nv12 codaj/outputs/test-sample.png
```
