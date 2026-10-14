# Regulus-RB SP500 Example

[Korean](README(KR).md)

## SP500 Vision DSP

SP500 is the Vision DSP in Regulus-RB.
It runs image and signal processing operations instead of the CA53 host.
The CA53 host application uses `libsp500` to send input buffers and operation commands to the DSP.
The SP500 firmware runs on FreeRTOS and processes operation requests from the host.
The public API provides letterbox, log-mel spectrogram, YOLO decoding, and postprocessing operations.
The primary DSP operation APIs return status codes. The host application uses these codes to check the operation results.

## Example

This example uses the SP500 DSP to apply letterbox processing to an input image. The output size is 640x640.
Letterbox processing changes the image size without changing its aspect ratio.
It fills the remaining area with gray pixels.

The processing sequence is:

1. Read `test-sample-1080p.png` with OpenCV.
2. Connect to the DSP with `libsp500` and allocate a shared output buffer.
3. Run the letterbox operation on SP500.
4. Save the result to `outputs/letterbox_out.png`.

## Build

Run these commands from the repository root directory on the development PC.
By default, `make` selects the latest valid cross toolchain in `/opt/crosstools/mobilint/2.0.0`.

```bash
cd regulus-rb/sp500
make
```

To use a specific cross toolchain, set `TOOLCHAIN_ROOT`.
Replace `X.Y.Z` with an installed cross-toolchain version.

```bash
make clean
make TOOLCHAIN_ROOT=/opt/crosstools/mobilint/2.0.0/vX.Y.Z
```

The build creates the ARM64 executable `sp500_letterbox`.

## Transfer to the Board

Replace `<BOARD_IP>` with the board IP address.

```bash
ssh root@<BOARD_IP> 'mkdir -p /root/sp500'
scp sp500_letterbox test-sample-1080p.png root@<BOARD_IP>:/root/sp500/
```

## Load the DSP Firmware

The SP500 program requires DSP firmware.
Run this command one time after you turn on or restart the board:

```bash
ssh root@<BOARD_IP>
dsp_firmware_load
```

The firmware loader shows messages similar to these. The exact messages can be different for each SDK version.

```text
SP500 reset assert
SP500 ptcm     loading complete: <BYTES> bytes from /usr/lib/firmware/mobilint/sp500/ptcm.bin
SP500 dtcm     loading complete: <BYTES> bytes from /usr/lib/firmware/mobilint/sp500/dtcm.bin
SP500 external loading complete: <BYTES> bytes from /usr/lib/firmware/mobilint/sp500/external.bin
SP500 reset de-assert; DSP running
```

Run the SP500 program only after the firmware loads without an error message.

## Run

```bash
cd /root/sp500
./sp500_letterbox test-sample-1080p.png
```

After a successful operation, the program creates `outputs/letterbox_out.png`.

## More Examples

For more preprocessing and postprocessing examples, refer to [regulus-dsp-examples](https://github.com/mobilint/regulus-dsp-examples).
