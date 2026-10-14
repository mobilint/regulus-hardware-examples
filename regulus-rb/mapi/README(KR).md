# Regulus-RB MAPI Examples

[English](README.md)

Regulus-RB의 MAPI 하드웨어를 사용하여 이미지의 크기와 픽셀 형식을 변환하는 C++ 예제입니다.

## MAPI란 무엇인가요?

MAPI는 압축이 풀린 이미지의 픽셀 데이터를 변환하는 하드웨어입니다.
CPU 대신 MAPI를 사용하면 이미지 크기 변경과 RGB/YUV 변환을 하드웨어로 처리할 수 있습니다.

MAPI 드라이버가 제공하는 주요 기능은 다음과 같습니다.

- 이미지 확대 및 축소
- RGB와 YUV 사이의 색공간 변환(Color Space Conversion, CSC)
- RGB/YUV 픽셀 형식 변환
- YUV 4:4:4, 4:2:2, 4:2:0 변환
- Bilinear, Cubic, Lanczos-2 스케일 필터 선택
- 사용자 CSC 계수와 luma/chroma phase 설정

PNG와 JPEG는 압축된 이미지 파일입니다.
MAPI는 이러한 파일을 직접 읽거나 저장하지 않으며, 압축이 풀린 RGB24, NV12, YUYV 등의 uncompressed pixel data를 처리합니다.
이 예제에서는 GStreamer가 PNG 압축 해제와 압축을 담당합니다.

## 예제

### `mapi_scale`

640x640 PNG 이미지를 읽어 MAPI에서 1024x1024로 확대하고 PNG로 저장합니다.

```text
test-sample.png
    -> GStreamer PNG decoder
    -> RGB 640x640
    -> MAPI scaler
    -> RGB 1024x1024
    -> GStreamer PNG encoder
    -> outputs/result_mapi_scale.png
```

MAPI 스케일 필터는 Cubic sharpness 1.0을 사용합니다.

### `mapi_csc`

640x640 NV12 형식의 uncompressed pixel data를 읽어 MAPI에서 RGB로 변환하고 PNG로 저장합니다.

```text
test-sample2.nv12
    -> GStreamer rawvideoparse
    -> NV12 640x640
    -> MAPI CSC
    -> RGB 640x640
    -> GStreamer PNG encoder
    -> outputs/result_mapi_csc.png
```

NV12 파일에는 해상도와 픽셀 형식 정보가 포함되어 있지 않습니다.
따라서 이 예제는 입력을 640x640 NV12로 가정합니다.
해당 uncompressed pixel data의 크기는 다음과 같습니다.

```text
640 x 640 x 1.5 = 614400 bytes
```

## 파일 구성

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

빌드하면 `mapi_scale`과 `mapi_csc` 실행 파일이 생성됩니다.
프로그램을 실행하면 `outputs` 디렉터리와 결과 이미지가 생성됩니다.

## 빌드

Makefile은 `/opt/crosstools/mobilint/2.0.0` 아래에서 환경 설정 파일이 있는 최신 유효 툴체인을 자동으로 선택합니다.

저장소 최상위 디렉터리에서 `mapi` 디렉터리로 이동한 후 빌드합니다.

```bash
cd regulus-rb/mapi
make
```

특정 버전을 사용하려면 `TOOLCHAIN_ROOT`를 지정합니다.
`X.Y.Z`를 설치된 크로스 툴체인 버전으로 바꾸십시오.

```bash
make clean
make TOOLCHAIN_ROOT=/opt/crosstools/mobilint/2.0.0/vX.Y.Z
```

다음 실행 파일이 생성됩니다.

```text
mapi_scale
mapi_csc
```

## 보드로 전송

저장소 최상위 디렉터리에서 `regulus-rb` 디렉터리로 이동합니다.

```bash
cd regulus-rb
```

빌드한 실행 파일과 테스트 입력을 포함하는 `mapi` 디렉터리 전체를 보드로 전송합니다.

```bash
scp -r mapi root@<BOARD_IP>:/root/
```

전송 후 보드의 디렉터리 구성은 다음과 같습니다.

```text
/root/mapi/
```

`<BOARD_IP>`는 사용하는 Regulus-RB 보드의 IP 주소로 바꾸어 입력합니다.

## MAPI 장치 확인

보드에 접속합니다.

```bash
ssh root@<BOARD_IP>
```

보드에 등록된 V4L2 장치와 비디오 노드를 확인합니다.

```bash
v4l2-ctl --list-devices
```

MAPI 비디오 노드는 제품 구성에 따라 달라질 수 있습니다.
출력에서 `C&M MAPI` 항목을 찾아 그 아래의 `/dev/videoN`을 확인합니다.

```text
C&M MAPI (platform:regulus-mapi):
    /dev/videoN
    /dev/mediaN
```

예를 들어 한 제품에서는 `/dev/video0`, 다른 제품에서는 `/dev/video1`로 표시될 수 있습니다.
이후 실행 명령에는 여기서 확인한 MAPI 비디오 노드를 입력합니다.

## 실행

보드에서 전송한 디렉터리로 이동합니다.

```bash
cd /root/mapi
```

이미지 확대 예제를 실행합니다.
마지막 인자에는 앞에서 확인한 MAPI 비디오 노드를 입력합니다.

```bash
./mapi_scale test-sample.png /dev/videoN
```

CSC 예제를 실행합니다.

```bash
./mapi_csc test-sample2.nv12 /dev/videoN
```

MAPI가 `/dev/video0`으로 확인되었다면 다음과 같이 실행합니다.

```bash
./mapi_scale test-sample.png /dev/video0
./mapi_csc test-sample2.nv12 /dev/video0
```

프로그램은 `/dev/videoN`에서 `v4l2videoNconvert` GStreamer 요소 이름을 만들어 해당 MAPI 장치를 사용합니다.

생성된 결과를 확인합니다.

```bash
file outputs/result_mapi_scale.png
file outputs/result_mapi_csc.png
```

예상 결과는 다음과 같습니다.

| 결과 파일 | 크기 | 픽셀 형식 |
| --- | ---: | --- |
| `outputs/result_mapi_scale.png` | 1024x1024 | RGB |
| `outputs/result_mapi_csc.png` | 640x640 | RGB |

## 결과를 개발 PC로 가져오기

개발 PC에서 저장소의 `regulus-rb` 디렉터리로 이동한 후 보드의 `outputs` 디렉터리를 가져옵니다.

```bash
cd regulus-rb
scp -r root@<BOARD_IP>:/root/mapi/outputs mapi/
```

결과는 개발 PC의 다음 디렉터리에 저장됩니다.

```text
regulus-rb/mapi/outputs/
```
