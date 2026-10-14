# Regulus-RB DMA350 Examples

[Korean](README(KR).md)

These C++ examples use the Regulus-RB DMA350 hardware to copy image data or change its layout.

## What Is DMA350?

DMA350 moves memory data without CPU processing.
For image data, DMA350 controls the read and write order of the pixels. It can mirror, rotate, and crop an image while it copies the data.

The Regulus-RB DMA350 driver provides these modes:

| Mode | Function |
| ---: | --- |
| 0 | 1D copy |
| 1 | 2D copy |
| 2 | Mirror |
| 3 | Rotation |
| 4 | Crop |

PNG is a compressed image file format.
DMA350 does not directly process PNG files. It processes uncompressed RGB pixel data.
In these examples, GStreamer decompresses and compresses the PNG data. The selected V4L2 converter element uses the DMA350 hardware.

## Examples

### `dma350_copy`

This example copies a 640x640 RGB image in DMA350.
Select `1d` or `2d` with a command argument.

```text
test-sample.png
    -> GStreamer PNG decoder
    -> RGB 640x640
    -> DMA350 1D copy or 2D copy
    -> RGB 640x640
    -> GStreamer PNG encoder
```

The 1D copy mode copies `640 x 640 x 3 = 1228800` bytes as one continuous block.
The 2D copy mode copies 1920 bytes in one row and repeats this operation 640 times.
The two output images are identical to the input image, but DMA350 uses a different mode for each operation.

The output files are:

```text
outputs/result_dma350_copy_1d.png
outputs/result_dma350_copy_2d.png
```

### `dma350_mirror`

This example mirrors a 640x640 RGB image from left to right.

```text
test-sample.png
    -> RGB 640x640
    -> DMA350 horizontal mirror
    -> RGB 640x640
    -> outputs/result_dma350_mirror.png
```

### `dma350_rotation`

This example rotates a 640x640 RGB image 90 degrees clockwise.

```text
test-sample.png
    -> RGB 640x640
    -> DMA350 clockwise 90-degree rotation
    -> RGB 640x640
    -> outputs/result_dma350_rotation.png
```

### `dma350_crop`

This example extracts a 320x320 area that starts at `(160, 160)` in the 640x640 image.

```text
test-sample.png
    -> RGB 640x640
    -> DMA350 center crop
    -> RGB 320x320
    -> outputs/result_dma350_crop.png
```

All pipelines use this property:

```text
disable-passthrough=true
```

This property prevents GStreamer from bypassing the conversion element when the input and output pixel formats are the same. Thus, the pipeline uses DMA350.

## Files

```text
dma350/
├── Makefile
├── README.md
├── README(KR).md
├── dma350_copy.cc
├── dma350_mirror.cc
├── dma350_rotation.cc
├── dma350_crop.cc
└── test-sample.png
```

The build creates four executables.
The programs create the `outputs` directory and the output images.

## Build

The Makefile automatically selects the latest valid cross toolchain in `/opt/crosstools/mobilint/2.0.0`.

Go to the `dma350` directory from the repository root directory. Then, build the examples.

```bash
cd regulus-rb/dma350
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
dma350_copy
dma350_mirror
dma350_rotation
dma350_crop
```

## Transfer to the Board

Go to the `regulus-rb` directory from the repository root directory.

```bash
cd regulus-rb
```

Transfer the complete `dma350` directory. The directory contains the executables and test input.

```bash
scp -r dma350 root@<BOARD_IP>:/root/
```

The directory on the board is:

```text
/root/dma350/
```

Replace `<BOARD_IP>` with the IP address of the Regulus-RB board.

## Check the DMA350 Device

Connect to the board.

```bash
ssh root@<BOARD_IP>
```

Show the registered V4L2 devices and video nodes.

```bash
v4l2-ctl --list-devices
```

Find `dma350` and the `/dev/videoN` node below it.
The video node number can be different for each product configuration.
Use this node when you run the examples.

The program checks the `/dev/videoN` format and finds a V4L2 converter for the node.
It does not check whether the node is a DMA350 device.
If you specify another converter node, such as a MAPI node, the pipeline can fail when it applies the DMA350 controls.
Always use the node below `dma350` in the `v4l2-ctl --list-devices` output.

## Run

Go to the transferred directory on the board.

```bash
cd /root/dma350
```

Run the 1D copy and 2D copy operations.

```bash
./dma350_copy test-sample.png /dev/videoN 1d
./dma350_copy test-sample.png /dev/videoN 2d
```

Run the mirror, rotation, and crop examples.

```bash
./dma350_mirror test-sample.png /dev/videoN
./dma350_rotation test-sample.png /dev/videoN
./dma350_crop test-sample.png /dev/videoN
```

Inspect the output files.

```bash
file outputs/*.png
```

The expected results are:

| Output file | Resolution | Operation |
| --- | ---: | --- |
| `result_dma350_copy_1d.png` | 640x640 | 1D copy |
| `result_dma350_copy_2d.png` | 640x640 | 2D copy |
| `result_dma350_mirror.png` | 640x640 | Horizontal mirror |
| `result_dma350_rotation.png` | 640x640 | 90-degree clockwise rotation |
| `result_dma350_crop.png` | 320x320 | Center crop |

## Copy the Results to the Development PC

On the development PC, go to the `regulus-rb` directory. Then, copy the `outputs` directory from the board.

```bash
cd regulus-rb
scp -r root@<BOARD_IP>:/root/dma350/outputs dma350/
```

The results are in this directory on the development PC:

```text
regulus-rb/dma350/outputs/
```
