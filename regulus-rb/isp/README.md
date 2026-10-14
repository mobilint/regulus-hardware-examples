# Regulus-RB ISP Camera Capture Example

[Korean](README(KR).md)

This example processes images from a MIPI camera in the ISP. It saves five PNG images.
It does not use a stored test video.

## What Is an ISP?

ISP means Image Signal Processor.
The ISP converts RAW data from an image sensor into pixel data that a display or codec can use.

The IMX678 sensor in this example outputs Bayer RAW data.
The input format on the board is `SRGGB12_1X12`.
In `full` mode, the ISP converts this input into 1920x1080 YUY2 images.

The ISP provides these functions:

- Bayer RAW demosaic
- Black level correction
- Color correction and YUV output conversion
- Crop and image subsampling
- Horizontal and vertical flip

This example does not directly set AE or AWB.

Crop selects a specified rectangular area from the input image.
The `center-crop` mode selects a 512x512 area from the center of the 1920x1080 image.

Image subsampling decreases the spatial resolution of the complete image.
It is different from chroma subsampling, such as YUV 4:2:0.
The `subsample-half` mode decreases the horizontal and vertical sample counts by 1/2.
The output size is 960x540.

The `flip-horizontal` mode mirrors the image from left to right.
The `flip-vertical` mode flips the image from top to bottom.

## Data Flow

```text
IMX678 MIPI camera
    -> CSI2-R0
    -> Bayer RAW, SRGGB12
    -> BTree ISP
    -> ISP crop, subsampling, or flip
    -> YUY2, 30 fps
    -> GStreamer PNG encoder
    -> five PNG images
```

The ISP processes the data from RAW input to YUY2 output.
GStreamer converts YUY2 to RGB and makes the PNG files.
PNG encoding is not an ISP function.

## Files

```text
isp/
├── Makefile
├── README.md
├── README(KR).md
└── isp_capture.cc
```

## Preparation

Connect an IMX678 MIPI camera to the CSI2-R0 port of the Regulus-RB board.
The ISP node is available only when the camera is connected.

## Check the ISP Device

Connect to the board.

```bash
ssh root@<BOARD_IP>
```

Show the V4L2 devices.

```bash
v4l2-ctl --list-devices
```

Find an ISP entry similar to this entry:

```text
BTree ISP (platform:regulus-isp):
    /dev/videoN
    /dev/mediaN
```

The video node number can be different for each board configuration.
Replace `/dev/videoN` in the commands below with the ISP node from this output.

Show the media graph.

```bash
media-ctl -d /dev/mediaN -p
```

IMX678, CSI2-R0, and the ISP must occur in this sequence:

```text
imx678 -> regulus-csi2-rx0 -> regulus-isp0
```

## Build

The Makefile selects the latest valid cross toolchain in `/opt/crosstools/mobilint/2.0.0`.

```bash
cd regulus-rb/isp
make
```

To use a specific cross-toolchain version, set `TOOLCHAIN_ROOT`.

```bash
make clean
make TOOLCHAIN_ROOT=/opt/crosstools/mobilint/2.0.0/vX.Y.Z
```

The build creates this executable:

```text
isp_capture
```

## Transfer to the Board

Run this command from the `regulus-rb` directory in the repository:

```bash
scp -r isp root@<BOARD_IP>:/root/
```

## Run

Go to the ISP directory on the board.

```bash
cd /root/isp
```

If you do not specify a mode, the program captures 1920x1080 images.

```bash
./isp_capture /dev/videoN
```

Apply 1/2 image subsampling.

```bash
./isp_capture /dev/videoN subsample-half
```

Crop a 512x512 area from the center of the 1920x1080 image.

```bash
./isp_capture /dev/videoN center-crop
```

Mirror the image from left to right.

```bash
./isp_capture /dev/videoN flip-horizontal
```

Flip the image from top to bottom.

```bash
./isp_capture /dev/videoN flip-vertical
```

Each command creates five images.
If you run the same mode again, the program replaces the existing files.

| Mode | Output size | Output files |
| --- | ---: | --- |
| `full` | 1920x1080 | `outputs/full_01.png` ... `full_05.png` |
| `subsample-half` | 960x540 | `outputs/subsample_half_01.png` ... `subsample_half_05.png` |
| `center-crop` | 512x512 | `outputs/center_crop_01.png` ... `center_crop_05.png` |
| `flip-horizontal` | 1920x1080 | `outputs/flip_horizontal_01.png` ... `flip_horizontal_05.png` |
| `flip-vertical` | 1920x1080 | `outputs/flip_vertical_01.png` ... `flip_vertical_05.png` |

Inspect the results.

```bash
file outputs/*.png
```
