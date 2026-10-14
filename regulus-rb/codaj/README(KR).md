# Regulus-RB CODAJ Examples

[English](README.md)

Regulus-RB의 CODAJ 하드웨어를 사용하여 JPEG 이미지를 압축하거나 압축 해제하는 C++ 예제입니다.

## CODAJ란 무엇인가요?

CODAJ는 JPEG/MJPEG 전용 하드웨어 코덱입니다.
CPU 대신 CODAJ를 사용하면 JPEG 압축과 압축 해제를 하드웨어로 처리할 수 있습니다.

- JPEG 인코더: NV12와 같은 압축되지 않은 픽셀 데이터를 JPEG로 압축합니다.
- JPEG 디코더: JPEG를 NV12와 같은 압축되지 않은 픽셀 데이터로 풉니다.

## 예제

### `codaj_decode`

640x640 JPEG 이미지를 CODAJ에서 디코딩하여 NV12 파일로 저장합니다.

```text
test-sample.jpg
    -> GStreamer JPEG parser
    -> CODAJ JPEG decoder
    -> NV12 640x640
    -> outputs/test-sample.nv12
```

### `codaj_encode`

640x640 NV12 파일을 CODAJ에서 인코딩하여 JPEG 이미지로 저장합니다.

```text
test-sample2.nv12
    -> GStreamer raw video parser
    -> NV12 640x640
    -> CODAJ JPEG encoder
    -> outputs/test-sample2.jpeg
```

NV12 파일에는 해상도와 픽셀 형식 정보가 포함되어 있지 않습니다.
따라서 이 예제는 NV12 입력을 640x640 해상도로 가정합니다.
파일 크기는 다음과 같습니다.

```text
640 x 640 x 1.5 = 614400 bytes
```

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

빌드하면 `codaj_decode`와 `codaj_encode` 실행 파일이 생성됩니다.
프로그램을 실행하면 `outputs` 디렉터리와 결과 파일이 생성됩니다.

## 빌드

Makefile은 `/opt/crosstools/mobilint/2.0.0` 아래에서 환경 설정 파일이 있는 최신 유효 툴체인을 자동으로 선택합니다.

저장소 최상위 디렉터리에서 `codaj` 디렉터리로 이동한 후 빌드합니다.

```bash
cd regulus-rb/codaj
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
codaj_decode
codaj_encode
```

## 보드로 전송

저장소 최상위 디렉터리에서 `regulus-rb` 디렉터리로 이동합니다.

```bash
cd regulus-rb
```

빌드한 실행 파일과 테스트 입력을 포함하는 `codaj` 디렉터리 전체를 보드로 전송합니다.

```bash
scp -r codaj root@<BOARD_IP>:/root/
```

전송 후 보드의 디렉터리 구성은 다음과 같습니다.

```text
/root/codaj/
```

`<BOARD_IP>`는 사용하는 Regulus-RB 보드의 IP 주소로 바꾸어 입력합니다.

## CODAJ 장치 확인

보드에 접속합니다.

```bash
ssh root@<BOARD_IP>
```

보드에 등록된 V4L2 장치와 비디오 노드를 확인합니다.

```bash
v4l2-ctl --list-devices
```

출력에서 `C&M CODAJ` 항목과 그 아래의 두 `/dev/videoN`을 확인합니다.
한 비디오 노드는 JPEG 인코더이고 다른 비디오 노드는 JPEG 디코더입니다.
비디오 노드 번호는 제품 구성에 따라 달라질 수 있습니다.

GStreamer가 선택한 인코더와 디코더 비디오 노드는 다음 명령으로 확인할 수 있습니다.

```bash
gst-inspect-1.0 v4l2jpegenc | grep 'Default: "/dev/video'
gst-inspect-1.0 v4l2jpegdec | grep 'Default: "/dev/video'
```

이 예제에서 사용하는 `v4l2jpegenc`와 `v4l2jpegdec` GStreamer 요소는 CODAJ 인코더와 디코더 비디오 노드를 자동으로 탐색합니다.
따라서 실행할 때 `/dev/videoN`을 별도로 입력하지 않습니다.

## 실행

보드에서 전송한 디렉터리로 이동합니다.

```bash
cd /root/codaj
```

JPEG 디코딩 예제를 실행합니다.

```bash
./codaj_decode test-sample.jpg
```

NV12 인코딩 예제를 실행합니다.

```bash
./codaj_encode test-sample2.nv12
```

생성된 결과를 확인합니다.

```bash
stat -c '%n: %s bytes' outputs/test-sample.nv12
file outputs/test-sample2.jpeg
```

예상 결과는 다음과 같습니다.

| 결과 파일 | 크기 또는 형식 |
| --- | --- |
| `outputs/test-sample.nv12` | 614400 bytes, NV12 640x640 |
| `outputs/test-sample2.jpeg` | JPEG image data, 640x640 |

## 결과를 개발 PC로 가져오기

개발 PC에서 저장소의 `regulus-rb` 디렉터리로 이동한 후 보드의 `outputs` 디렉터리를 가져옵니다.

```bash
cd regulus-rb
scp -r root@<BOARD_IP>:/root/codaj/outputs codaj/
```

결과는 개발 PC의 다음 디렉터리에 저장됩니다.

```text
regulus-rb/codaj/outputs/
```

JPEG 결과는 IDE에서 바로 열 수 있습니다.
NV12 결과를 확인하려면 해상도와 픽셀 형식을 지정하여 PNG로 변환합니다.

```bash
ffmpeg -f rawvideo -pixel_format nv12 -video_size 640x640 \
    -i codaj/outputs/test-sample.nv12 codaj/outputs/test-sample.png
```
