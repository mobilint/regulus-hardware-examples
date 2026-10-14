# Regulus-RB Wave5 Examples

[English](README.md)

Regulus-RB의 Wave5 하드웨어를 사용하여 H.264 동영상을 압축하거나 압축 해제하는 C++ 예제입니다.

## Wave5란 무엇인가요?

Wave5는 H.264/H.265 동영상 코덱을 처리하는 하드웨어입니다.
CPU 대신 Wave5를 사용하면 동영상 압축과 압축 해제를 하드웨어로 처리할 수 있습니다.

- 압축 해제: H.264/H.265 압축 영상을 NV12와 같은 압축되지 않은 픽셀 데이터로 풉니다.
- 압축: NV12와 같은 압축되지 않은 픽셀 데이터를 H.264/H.265 영상으로 압축합니다.

MP4는 영상과 음성 등을 담는 컨테이너입니다.
Wave5는 MP4 파일 분리와 결합을 처리하지 않습니다.
이 예제에서는 GStreamer가 MP4 파일 분리와 결합을 담당하고 Wave5가 H.264 압축 해제와 압축을 담당합니다.

## 예제

### `wave5_decode`

MP4 컨테이너에서 H.264 데이터를 꺼낸 후 Wave5에서 NV12 픽셀 데이터로 디코딩합니다.

```text
decode-sample.mp4
    -> GStreamer MP4 파일 분리
    -> H.264
    -> Wave5 decoder
    -> NV12
    -> outputs/decode-sample.nv12
```

GStreamer는 MP4에서 H.264 데이터를 분리합니다.
H.264 압축 해제는 Wave5가 수행합니다.

디코더가 출력하는 NV12 프레임들은 `decode-sample.nv12` 파일 하나에 순서대로 저장됩니다.

### `wave5_encode`

NV12 픽셀 데이터를 Wave5에서 H.264로 압축한 후 MP4 컨테이너에 저장합니다.

```text
encode-sample.nv12
    -> NV12 640x640, 30 FPS
    -> Wave5 encoder
    -> H.264
    -> GStreamer MP4 파일 결합
    -> outputs/encode-sample.mp4
```

Wave5는 H.264 압축을 수행합니다.
GStreamer는 H.264 데이터를 MP4 컨테이너에 저장합니다.

NV12 파일에는 픽셀 형식, 해상도, 프레임 속도 정보가 포함되어 있지 않습니다.
따라서 이 예제는 입력을 NV12 640x640, 30 FPS로 해석합니다.

## 파일 구성

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

빌드하면 `wave5_decode`와 `wave5_encode` 실행 파일이 생성됩니다.
프로그램을 실행하면 `outputs` 디렉터리와 결과 파일이 생성됩니다.

## 빌드

Makefile은 `/opt/crosstools/mobilint/2.0.0` 아래에서 환경 설정 파일이 있는 최신 유효 툴체인을 자동으로 선택합니다.

저장소 최상위 디렉터리에서 `wave5` 디렉터리로 이동한 후 빌드합니다.

```bash
cd regulus-rb/wave5
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
wave5_decode
wave5_encode
```

## 보드로 전송

저장소 최상위 디렉터리에서 `regulus-rb` 디렉터리로 이동합니다.

```bash
cd regulus-rb
```

빌드한 실행 파일과 테스트 입력을 포함하는 `wave5` 디렉터리 전체를 보드로 전송합니다.

```bash
scp -r wave5 root@<BOARD_IP>:/root/
```

전송 후 보드의 디렉터리 구성은 다음과 같습니다.

```text
/root/wave5/
```

`<BOARD_IP>`는 사용하는 Regulus-RB 보드의 IP 주소로 바꾸어 입력합니다.

## Wave5 장치 확인

보드에 접속합니다.

```bash
ssh root@<BOARD_IP>
```

보드에 등록된 V4L2 장치와 비디오 노드를 확인합니다.

```bash
v4l2-ctl --list-devices
```

출력에서 Wave5 디코더와 인코더 항목을 확인합니다.
Wave5 비디오 노드 번호는 제품 구성에 따라 달라질 수 있습니다.

GStreamer가 선택한 Wave5 디코더와 인코더 비디오 노드를 확인합니다.

```bash
gst-inspect-1.0 v4l2h264dec | grep 'Default: "/dev/video'
gst-inspect-1.0 v4l2h264enc | grep 'Default: "/dev/video'
```

`v4l2h264dec`와 `v4l2h264enc`는 Wave5 비디오 노드를 자동으로 탐색합니다.
따라서 실행할 때 `/dev/videoN`을 별도로 입력하지 않습니다.

## 실행

보드에서 전송한 디렉터리로 이동합니다.

```bash
cd /root/wave5
```

H.264 디코딩 예제를 실행합니다.

```bash
./wave5_decode decode-sample.mp4
```

NV12 인코딩 예제를 실행합니다.

```bash
./wave5_encode encode-sample.nv12
```

각 프로그램은 처리 완료를 최대 90초 동안 기다립니다.
이 제한은 제공된 샘플을 기준으로 합니다.
처리가 90초를 초과하면 프로그램이 오류로 종료합니다.

생성된 결과를 확인합니다.

```bash
stat -c '%n: %s bytes' outputs/decode-sample.nv12
file outputs/encode-sample.mp4
```

예상 결과는 다음과 같습니다.

| 결과 파일 | 형식 |
| --- | --- |
| `outputs/decode-sample.nv12` | NV12 640x640 raw video |
| `outputs/encode-sample.mp4` | H.264 MP4, 640x640, 30 FPS |

## 결과를 개발 PC로 가져오기

개발 PC에서 저장소의 `regulus-rb` 디렉터리로 이동한 후 보드의 `outputs` 디렉터리를 가져옵니다.

```bash
cd regulus-rb
scp -r root@<BOARD_IP>:/root/wave5/outputs wave5/
```

결과는 개발 PC의 다음 디렉터리에 저장됩니다.

```text
regulus-rb/wave5/outputs/
```

MP4 결과는 IDE 또는 일반 동영상 플레이어에서 바로 확인할 수 있습니다.
