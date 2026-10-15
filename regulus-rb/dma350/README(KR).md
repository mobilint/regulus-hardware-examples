# Regulus-RB DMA350 Examples

[English](README.md)

Regulus-RB의 DMA350 하드웨어를 사용하여 이미지 데이터를 복사하거나 배치를 변경하는 C++ 예제입니다.

## DMA350이란 무엇인가요?

DMA350은 CPU 대신 메모리 데이터를 이동하는 하드웨어입니다.
이미지에서는 픽셀 데이터를 복사하면서 읽고 쓰는 순서를 조절하여 반전, 회전 및 자르기(crop)를 수행할 수 있습니다.

Regulus-RB DMA350 드라이버가 제공하는 모드는 다음과 같습니다.

| 모드 | 기능 |
| ---: | --- |
| 0 | 1D copy |
| 1 | 2D copy |
| 2 | Mirror |
| 3 | Rotation |
| 4 | 자르기 |

PNG는 압축된 이미지 파일입니다.
DMA350은 PNG 파일을 직접 처리하지 않고 비압축 RGB 픽셀 데이터를 처리합니다.
이 예제에서는 GStreamer가 PNG 압축 해제와 압축을 담당합니다. 선택한 V4L2 변환 요소가 DMA350 하드웨어를 사용합니다.

## 예제

### `dma350_copy`

640x640 RGB 이미지를 DMA350에서 복사합니다.
실행 인자로 `1d` 또는 `2d`를 선택합니다.

```text
test-sample.png
    -> GStreamer PNG 디코더
    -> RGB 640x640
    -> DMA350 1D copy 또는 2D copy
    -> RGB 640x640
    -> GStreamer PNG 인코더
```

1D copy는 `640 x 640 x 3 = 1228800`바이트를 하나의 연속된 데이터로 복사합니다.
2D copy는 한 행의 1920바이트를 640번 복사합니다.
두 결과 이미지는 원본과 동일하지만 DMA350에 설정되는 모드가 다릅니다.

결과 파일은 다음과 같습니다.

```text
outputs/result_dma350_copy_1d.png
outputs/result_dma350_copy_2d.png
```

### `dma350_mirror`

640x640 RGB 이미지를 좌우로 반전합니다.

```text
test-sample.png
    -> RGB 640x640
    -> DMA350 horizontal mirror
    -> RGB 640x640
    -> outputs/result_dma350_mirror.png
```

### `dma350_rotation`

640x640 RGB 이미지를 시계 방향으로 90도 회전합니다.

```text
test-sample.png
    -> RGB 640x640
    -> DMA350 clockwise 90-degree rotation
    -> RGB 640x640
    -> outputs/result_dma350_rotation.png
```

### `dma350_crop`

640x640 이미지의 `(160, 160)` 위치에서 시작하는 320x320 영역을 추출합니다.

```text
test-sample.png
    -> RGB 640x640
    -> DMA350 중앙 영역 자르기
    -> RGB 320x320
    -> outputs/result_dma350_crop.png
```

모든 파이프라인은 다음 속성을 사용합니다.

```text
disable-passthrough=true
```

입력과 출력 픽셀 형식이 같아도 GStreamer가 변환 요소를 건너뛰지 않고 DMA350을 사용하도록 하는 설정입니다.

## 파일 구성

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

빌드하면 네 실행 파일이 생성됩니다.
프로그램을 실행하면 `outputs` 디렉터리와 결과 이미지가 생성됩니다.

## 빌드

Makefile은 `/opt/crosstools/mobilint/2.0.0` 아래에서 환경 설정 파일이 있는 최신 유효 툴체인을 자동으로 선택합니다.

저장소 최상위 디렉터리에서 `dma350` 디렉터리로 이동한 후 빌드합니다.

```bash
cd regulus-rb/dma350
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
dma350_copy
dma350_mirror
dma350_rotation
dma350_crop
```

## 보드로 전송

저장소 최상위 디렉터리에서 `regulus-rb` 디렉터리로 이동합니다.

```bash
cd regulus-rb
```

빌드한 실행 파일과 테스트 입력을 포함하는 `dma350` 디렉터리 전체를 보드로 전송합니다.

```bash
scp -r dma350 root@<BOARD_IP>:/root/
```

전송 후 보드의 디렉터리 구성은 다음과 같습니다.

```text
/root/dma350/
```

`<BOARD_IP>`는 사용하는 Regulus-RB 보드의 IP 주소로 바꾸어 입력합니다.

## DMA350 장치 확인

보드에 접속합니다.

```bash
ssh root@<BOARD_IP>
```

보드에 등록된 V4L2 장치와 비디오 노드를 확인합니다.

```bash
v4l2-ctl --list-devices
```

출력에서 `dma350` 항목과 그 아래의 `/dev/videoN`을 확인합니다.
비디오 노드 번호는 제품 구성에 따라 달라질 수 있습니다.
예제를 실행할 때 이 노드를 입력합니다.

프로그램은 `/dev/videoN` 형식과 해당 노드의 V4L2 변환 요소를 확인합니다.
해당 노드가 DMA350 장치인지는 확인하지 않습니다.
MAPI와 같은 다른 변환 노드를 입력하면 DMA350 제어값을 적용할 때 파이프라인이 실패할 수 있습니다.
항상 `v4l2-ctl --list-devices` 출력의 `dma350` 아래에 있는 노드를 사용합니다.

## 실행

보드에서 전송한 디렉터리로 이동합니다.

```bash
cd /root/dma350
```

1D copy와 2D copy를 각각 실행합니다.

```bash
./dma350_copy test-sample.png /dev/videoN 1d
./dma350_copy test-sample.png /dev/videoN 2d
```

반전, 회전 및 자르기 예제를 실행합니다.

```bash
./dma350_mirror test-sample.png /dev/videoN
./dma350_rotation test-sample.png /dev/videoN
./dma350_crop test-sample.png /dev/videoN
```

생성된 결과를 확인합니다.

```bash
file outputs/*.png
```

예상 결과는 다음과 같습니다.

| 결과 파일 | 해상도 | 동작 |
| --- | ---: | --- |
| `result_dma350_copy_1d.png` | 640x640 | 1D copy |
| `result_dma350_copy_2d.png` | 640x640 | 2D copy |
| `result_dma350_mirror.png` | 640x640 | 좌우 반전 |
| `result_dma350_rotation.png` | 640x640 | 시계 방향 90도 회전 |
| `result_dma350_crop.png` | 320x320 | 중앙 영역 자르기 |

## 결과를 개발 PC로 가져오기

개발 PC에서 저장소의 `regulus-rb` 디렉터리로 이동한 후 보드의 `outputs` 디렉터리를 가져옵니다.

```bash
cd regulus-rb
scp -r root@<BOARD_IP>:/root/dma350/outputs dma350/
```

결과는 개발 PC의 다음 디렉터리에 저장됩니다.

```text
regulus-rb/dma350/outputs/
```
