# Regulus-RA ISP Camera Capture Example

[English](README.md)

이 예제는 MIPI 카메라 영상을 ISP로 처리하고 PNG 이미지 5장으로 저장합니다.
저장된 테스트 영상은 사용하지 않습니다.

## ISP란 무엇인가요?

ISP는 Image Signal Processor의 약어입니다.
ISP는 이미지 센서의 RAW 데이터를 화면 또는 코덱이 사용할 수 있는 픽셀 데이터로 바꿉니다.

이 예제의 IMX678 센서는 Bayer RAW 데이터를 출력합니다.
보드에서 확인되는 입력 형식은 `SRGGB12_1X12`입니다.
`full` 모드에서 ISP는 이 입력을 1920x1080 YUY2 영상으로 처리합니다.

ISP에는 다음과 같은 기능이 있습니다.

- Bayer RAW demosaic
- Black level 보정
- 색 보정과 YUV 출력 변환
- Crop과 image subsampling
- 수평 및 수직 반전

이 예제는 AE와 AWB를 직접 설정하지 않습니다.

Crop은 입력 영상에서 지정한 사각형 영역만 선택합니다.
`center-crop` 모드는 1920x1080 영상의 중앙에서 512x512 영역을 선택합니다.

Image subsampling은 전체 영상의 공간 해상도를 줄입니다.
이 기능은 YUV 4:2:0과 같은 chroma subsampling과 다릅니다.
`subsample-half` 모드는 가로와 세로의 샘플 수를 각각 1/2로 줄입니다.
따라서 출력 크기는 960x540입니다.

`flip-horizontal` 모드는 영상을 좌우로 반전합니다.
`flip-vertical` 모드는 영상을 상하로 반전합니다.

## 데이터 흐름

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

ISP는 RAW 입력부터 YUY2 출력까지 처리합니다.
GStreamer는 YUY2를 RGB로 변환하고 PNG 파일을 만듭니다.
PNG 변환은 ISP 기능이 아닙니다.

## 예제 파일

```text
isp/
├── Makefile
├── README.md
├── README(KR).md
└── isp_capture.cc
```

## 준비 사항

Regulus-RA 보드의 CSI2-R0 포트에 IMX678 MIPI 카메라를 연결합니다.
카메라를 연결해야 ISP 노드가 활성화되어 사용할 수 있습니다.

## ISP 장치 확인

보드에 접속합니다.

```bash
ssh root@<BOARD_IP>
```

V4L2 장치를 확인합니다.

```bash
v4l2-ctl --list-devices
```

다음과 같은 ISP 항목을 찾으십시오.

```text
BTree ISP (platform:regulus-isp):
    /dev/videoN
    /dev/mediaN
```

비디오 노드 번호는 보드 구성에 따라 달라질 수 있습니다.
아래 명령의 `/dev/videoN`을 실제 ISP 노드로 바꾸십시오.

Media graph를 확인합니다.

```bash
media-ctl -d /dev/mediaN -p
```

IMX678, CSI2-R0, ISP가 다음 순서로 표시되어야 합니다.

```text
imx678 -> regulus-csi2-rx0 -> regulus-isp0
```

## 빌드

Makefile은 `/opt/crosstools/mobilint/1.0.0` 아래에서 환경 설정 파일이 있는 최신 유효 툴체인을 선택합니다.

```bash
cd regulus-ra/isp
make
```

특정 툴체인 버전을 사용하려면 `TOOLCHAIN_ROOT`를 지정합니다.

```bash
make clean
make TOOLCHAIN_ROOT=/opt/crosstools/mobilint/1.0.0/vX.Y.Z
```

다음 실행 파일이 생성됩니다.

```text
isp_capture
```

## 보드로 전송

저장소의 `regulus-ra` 디렉터리에서 실행합니다.

```bash
scp -r isp root@<BOARD_IP>:/root/
```

## 실행

보드에서 ISP 디렉터리로 이동합니다.

```bash
cd /root/isp
```

모드를 지정하지 않으면 1920x1080 영상을 캡처합니다.

```bash
./isp_capture /dev/videoN
```

1/2 image subsampling을 적용합니다.

```bash
./isp_capture /dev/videoN subsample-half
```

1920x1080 영상의 중앙을 512x512 크기로 crop합니다.

```bash
./isp_capture /dev/videoN center-crop
```

영상을 좌우로 반전합니다.

```bash
./isp_capture /dev/videoN flip-horizontal
```

영상을 상하로 반전합니다.

```bash
./isp_capture /dev/videoN flip-vertical
```

각 명령은 이미지 5장을 생성합니다.
같은 모드를 다시 실행하면 기존 파일을 덮어씁니다.

| 모드 | 출력 크기 | 출력 파일 |
| --- | ---: | --- |
| `full` | 1920x1080 | `outputs/full_01.png` ... `full_05.png` |
| `subsample-half` | 960x540 | `outputs/subsample_half_01.png` ... `subsample_half_05.png` |
| `center-crop` | 512x512 | `outputs/center_crop_01.png` ... `center_crop_05.png` |
| `flip-horizontal` | 1920x1080 | `outputs/flip_horizontal_01.png` ... `flip_horizontal_05.png` |
| `flip-vertical` | 1920x1080 | `outputs/flip_vertical_01.png` ... `flip_vertical_05.png` |

결과를 확인합니다.

```bash
file outputs/*.png
```
