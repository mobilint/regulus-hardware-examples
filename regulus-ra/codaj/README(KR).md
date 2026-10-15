# Regulus-RA CODAJ Examples

[English](README.md)

Regulus-RA의 CODAJ 하드웨어를 사용하여 JPEG 이미지를 압축하거나 압축 해제하는 C++ 예제입니다.

## CODAJ란 무엇인가요?

CODAJ는 JPEG/MJPEG 전용 하드웨어 코덱입니다.

- JPEG 인코더는 NV12 픽셀 데이터를 JPEG로 압축합니다.
- JPEG 디코더는 JPEG를 NV12 픽셀 데이터로 풉니다.

## 예제

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

NV12 파일에는 해상도와 픽셀 형식 정보가 없으므로 이 예제는 입력을 640x640 NV12로 가정합니다.

NV12 파일의 크기는 `640 x 640 x 1.5 = 614400 bytes`입니다.

## 파일 구성

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

## 빌드

Makefile은 `/opt/crosstools/mobilint/1.0.0` 아래에서 환경 설정 파일이 있는 최신 유효 툴체인을 자동으로 선택합니다.

저장소 최상위 디렉터리에서 빌드합니다.

```bash
cd regulus-ra/codaj
make
```

특정 버전을 사용하려면 `TOOLCHAIN_ROOT`를 지정합니다.
`X.Y.Z`를 설치된 크로스 툴체인 버전으로 바꾸십시오.

```bash
make clean
make TOOLCHAIN_ROOT=/opt/crosstools/mobilint/1.0.0/vX.Y.Z
```

## 보드로 전송

개발 PC의 저장소 최상위 디렉터리에서 다음 명령을 실행합니다.

```bash
cd regulus-ra
scp -r codaj root@<BOARD_IP>:/root/
```

## CODAJ 장치 확인

보드에서 V4L2 장치를 확인합니다.

```bash
v4l2-ctl --list-devices
```

출력에서 `C&M CODAJ Encoder`와 그 아래의 인코더 및 디코더 비디오 노드를 확인합니다.

GStreamer가 선택한 비디오 노드는 다음 명령으로 확인할 수 있습니다.

```bash
gst-inspect-1.0 v4l2jpegenc | grep 'Default: "/dev/video'
gst-inspect-1.0 v4l2jpegdec | grep 'Default: "/dev/video'
```

`v4l2jpegenc`와 `v4l2jpegdec` 요소가 CODAJ 비디오 노드를 자동으로 찾으므로 실행할 때 비디오 노드를 입력하지 않습니다.

## 실행

```bash
cd /root/codaj
./codaj_decode test-sample.jpg
./codaj_encode test-sample2.nv12
```

생성된 결과를 확인합니다.

```bash
stat -c '%n: %s bytes' outputs/test-sample.nv12
file outputs/test-sample2.jpeg
```

## 결과를 개발 PC로 가져오기

개발 PC의 저장소 최상위 디렉터리에서 다음 명령을 실행합니다.

```bash
cd regulus-ra
scp -r root@<BOARD_IP>:/root/codaj/outputs codaj/
```
