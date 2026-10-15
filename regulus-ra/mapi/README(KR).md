# Regulus-RA MAPI Examples

[English](README.md)

Regulus-RA의 MAPI 하드웨어를 사용하여 이미지 크기와 픽셀 형식을 변환하는 C++ 예제입니다.

## MAPI란 무엇인가요?

MAPI는 비압축 픽셀 데이터를 변환하는 하드웨어입니다.

CPU 대신 MAPI를 사용하면 이미지 크기 변경과 RGB/YUV 변환을 하드웨어로 처리할 수 있습니다.

PNG 압축 해제와 결과 PNG 압축은 GStreamer가 처리합니다.

## 예제

### `mapi_scale`

640x640 PNG 이미지를 MAPI에서 1024x1024로 확대합니다.

```text
test-sample.png
    -> GStreamer PNG 디코더
    -> RGB 640x640
    -> MAPI scaler
    -> RGB 1024x1024
    -> GStreamer PNG 인코더
    -> outputs/result_mapi_scale.png
```

### `mapi_csc`

640x640 NV12 픽셀 데이터를 MAPI에서 RGB로 변환합니다.

```text
test-sample2.nv12
    -> GStreamer raw video parser
    -> NV12 640x640
    -> MAPI CSC
    -> RGB 640x640
    -> GStreamer PNG 인코더
    -> outputs/result_mapi_csc.png
```

NV12 파일에는 해상도와 픽셀 형식 정보가 없으므로 이 예제는 입력을 640x640 NV12로 가정합니다.

NV12 입력 파일의 크기는 `640 x 640 x 1.5 = 614400 bytes`입니다.

## 파일 구성

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

## 빌드

Makefile은 `/opt/crosstools/mobilint/1.0.0` 아래에서 환경 설정 파일이 있는 최신 유효 툴체인을 자동으로 선택합니다.

저장소 최상위 디렉터리에서 빌드합니다.

```bash
cd regulus-ra/mapi
make
```

특정 버전을 사용하려면 `TOOLCHAIN_ROOT`를 지정합니다.
`X.Y.Z`를 설치된 크로스 툴체인 버전으로 바꾸십시오.

```bash
make clean
make TOOLCHAIN_ROOT=/opt/crosstools/mobilint/1.0.0/vX.Y.Z
```

## 보드로 전송

저장소 최상위 디렉터리에서 `regulus-ra` 디렉터리로 이동합니다.

```bash
cd regulus-ra
scp -r mapi root@<BOARD_IP>:/root/
```

## MAPI 장치 확인

보드에서 V4L2 장치를 확인합니다.

```bash
v4l2-ctl --list-devices
```

출력에서 `C&M MAPI`와 그 아래의 `/dev/videoN`을 확인합니다.

GStreamer가 선택한 MAPI 비디오 노드는 다음 명령으로 확인할 수 있습니다.

```bash
gst-inspect-1.0 v4l2convert | grep 'Default: "/dev/video'
```

Regulus-RA의 `v4l2convert` 요소가 MAPI 비디오 노드를 자동으로 찾으므로 실행할 때 비디오 노드를 입력하지 않습니다.

## 실행

```bash
cd /root/mapi
./mapi_scale test-sample.png
./mapi_csc test-sample2.nv12
```

생성된 결과를 확인합니다.

```bash
file outputs/result_mapi_scale.png
file outputs/result_mapi_csc.png
```

## 결과를 개발 PC로 가져오기

개발 PC의 저장소 최상위 디렉터리에서 실행합니다.

```bash
scp -r root@<BOARD_IP>:/root/mapi/outputs regulus-ra/mapi/
```
