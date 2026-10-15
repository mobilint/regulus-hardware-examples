# Regulus-RB Hardware Examples

[English](README.md)

Regulus-RB의 이미지, 동영상 및 데이터 처리 하드웨어를 사용하는 C++ 예제입니다.

## 하드웨어

| 컴포넌트 | 설명 |
| --- | --- |
| [MAPI](mapi/README(KR).md) | 이미지 크기와 RGB/YUV 픽셀 형식을 변환합니다. |
| [CODAJ](codaj/README(KR).md) | JPEG 이미지를 압축하거나 압축 해제합니다. |
| [DMA350](dma350/README(KR).md) | 메모리를 복사하고 이미지를 자르거나 회전·반전합니다. |
| [ISP](isp/README(KR).md) | MIPI 카메라의 RAW 센서 데이터를 처리하고 결과 영상을 캡처합니다. |
| [SP500](sp500/README(KR).md) | 영상 및 신호 처리 연산을 실행하는 Vision DSP입니다. |
| [Wave5](wave5/README(KR).md) | H.264/H.265 영상을 압축하거나 압축 해제합니다. |

각 예제의 빌드 및 실행 방법은 해당 디렉터리의 `README(KR).md`에서 확인할 수 있습니다.

## 빌드 준비

예제를 빌드하려면 개발 PC에 Regulus-RB SDK와 크로스 툴체인을 먼저 설치해야 합니다.

SDK는 [MOBILINT Download](https://dl.mobilint.com)에서 받을 수 있습니다.

각 예제의 Makefile은 `/opt/crosstools/mobilint/2.0.0` 아래에서 환경 설정 파일이 있는 최신 유효 툴체인을 자동으로 선택합니다.
따라서 환경 설정 스크립트를 실행하지 않아도 됩니다.

특정 버전을 사용하려면 예제 디렉터리에서 `TOOLCHAIN_ROOT`를 지정합니다.
`X.Y.Z`를 설치된 크로스 툴체인 버전으로 바꾸십시오.

```bash
make clean
make TOOLCHAIN_ROOT=/opt/crosstools/mobilint/2.0.0/vX.Y.Z
```

## V4L2 장치 확인

이 확인 방법은 MAPI, CODAJ, DMA350, ISP 및 Wave5에 적용됩니다.
DSP 펌웨어 절차는 SP500 예제를 참조하십시오.

보드의 V4L2 장치는 다음 명령으로 확인합니다.

```bash
v4l2-ctl --list-devices
```

`/dev/videoN`의 번호는 제품 구성에 따라 달라질 수 있습니다.
