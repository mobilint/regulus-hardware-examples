# Regulus-RA Wave5 Examples

[Korean](README(KR).md)

These C++ examples use the Regulus-RA Wave5 hardware to encode or decode H.264 video.

## What Is Wave5?

Wave5 is hardware that processes H.264/H.265 video codecs.
Wave5 encodes and decodes video in hardware instead of on the CPU.

- Decoding: Decompresses H.264/H.265 video into uncompressed pixel data, such as NV12 data.
- Encoding: Compresses uncompressed pixel data, such as NV12 data, into H.264/H.265 video.

MP4 is a container for video, audio, and other data.
Wave5 does not demultiplex or multiplex MP4 files.
In these examples, GStreamer processes the MP4 container, and Wave5 processes H.264 decoding or encoding.

## Examples

### `wave5_decode`

This example extracts H.264 data from an MP4 container. Then, Wave5 decodes the data into NV12 pixel data.

```text
decode-sample.mp4
    -> GStreamer MP4 demuxer
    -> H.264
    -> Wave5 decoder
    -> NV12
    -> outputs/decode-sample.nv12
```

GStreamer extracts the H.264 data from the MP4 container.
Wave5 decodes the H.264 data.

The program saves the NV12 frames from the decoder in sequence in one `decode-sample.nv12` file.

### `wave5_encode`

This example uses Wave5 to encode NV12 pixel data into H.264 data. Then, it saves the data in an MP4 container.

```text
encode-sample.nv12
    -> NV12 640x640, 30 FPS
    -> Wave5 encoder
    -> H.264
    -> GStreamer MP4 muxer
    -> outputs/encode-sample.mp4
```

Wave5 encodes the NV12 data into H.264 data.
GStreamer saves the H.264 data in the MP4 container.

An NV12 file does not contain pixel format, resolution, or frame rate information.
This example interprets the input as 640x640 NV12 data at 30 FPS.

## Files

```text
wave5/
├── Makefile
├── README.md
├── README(KR).md
├── wave5_decode.cc
├── wave5_encode.cc
├── decode-sample.mp4
└── encode-sample.nv12
```

The build creates the `wave5_decode` and `wave5_encode` executables.
The programs create the `outputs` directory and the output files.

## Build

The Makefile automatically selects the latest valid cross toolchain in `/opt/crosstools/mobilint/1.0.0`.

Go to the `wave5` directory from the repository root directory. Then, build the examples.

```bash
cd regulus-ra/wave5
make
```

To use a specific version, set `TOOLCHAIN_ROOT`.
Replace `X.Y.Z` with an installed cross-toolchain version.

```bash
make clean
make TOOLCHAIN_ROOT=/opt/crosstools/mobilint/1.0.0/vX.Y.Z
```

The build creates these executables:

```text
wave5_decode
wave5_encode
```

## Transfer to the Board

Go to the `regulus-ra` directory from the repository root directory.

```bash
cd regulus-ra
```

Transfer the complete `wave5` directory. The directory contains the executables and test inputs.

```bash
scp -r wave5 root@<BOARD_IP>:/root/
```

The directory on the board is:

```text
/root/wave5/
```

Replace `<BOARD_IP>` with the IP address of the Regulus-RA board.

## Check the Wave5 Device

Connect to the board.

```bash
ssh root@<BOARD_IP>
```

Show the registered V4L2 devices and video nodes.

```bash
v4l2-ctl --list-devices
```

Find the Wave5 decoder and encoder entries.
The Wave5 video node numbers can be different for each product configuration.

Use these commands to show the Wave5 decoder and encoder video nodes that GStreamer selected:

```bash
gst-inspect-1.0 v4l2h264dec | grep 'Default: "/dev/video'
gst-inspect-1.0 v4l2h264enc | grep 'Default: "/dev/video'
```

The `v4l2h264dec` and `v4l2h264enc` elements automatically find the Wave5 video nodes.
You do not specify a `/dev/videoN` node when you run these examples.

## Run

Go to the transferred directory on the board.

```bash
cd /root/wave5
```

Run the H.264 decoding example.

```bash
./wave5_decode decode-sample.mp4
```

Run the NV12 encoding example.

```bash
./wave5_encode encode-sample.nv12
```

Each program waits up to 90 seconds for processing to finish.
This limit applies to the supplied samples.
The program exits with an error if processing takes more than 90 seconds.

Inspect the output files.

```bash
stat -c '%n: %s bytes' outputs/decode-sample.nv12
file outputs/encode-sample.mp4
```

The expected results are:

| Output file | Format |
| --- | --- |
| `outputs/decode-sample.nv12` | NV12 640x640 raw video |
| `outputs/encode-sample.mp4` | H.264 MP4, 640x640, 30 FPS |

## Copy the Results to the Development PC

On the development PC, go to the `regulus-ra` directory. Then, copy the `outputs` directory from the board.

```bash
cd regulus-ra
scp -r root@<BOARD_IP>:/root/wave5/outputs wave5/
```

The results are in this directory on the development PC:

```text
regulus-ra/wave5/outputs/
```

You can open the MP4 result in the IDE or a video player.
