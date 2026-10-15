# Regulus-RA Hardware Examples

[English](README.md)

Regulus-RA의 이미지 및 동영상 처리 하드웨어를 사용하는 C++ 예제입니다.

## 하드웨어

| 컴포넌트 | 설명 |
| --- | --- |
| [MAPI](mapi/README(KR).md) | 이미지 크기와 RGB/YUV 픽셀 형식을 변환합니다. |
| [CODAJ](codaj/README(KR).md) | JPEG 이미지를 압축하거나 압축 해제합니다. |
| [ISP](isp/README(KR).md) | MIPI 카메라의 RAW 센서 데이터를 처리하고 결과 영상을 캡처합니다. |
| [Wave5](wave5/README(KR).md) | H.264/H.265 영상을 압축하거나 압축 해제합니다. |

각 예제의 빌드 및 실행 방법은 해당 디렉터리의 `README(KR).md`에서 확인할 수 있습니다.

## 빌드 준비

예제를 빌드하려면 개발 PC에 Regulus-RA SDK와 크로스 툴체인을 먼저 설치해야 합니다.

SDK는 [MOBILINT Download](https://dl.mobilint.com)에서 받을 수 있습니다.

각 예제의 Makefile은 `/opt/crosstools/mobilint/1.0.0` 아래에서 환경 설정 파일이 있는 최신 유효 툴체인을 자동으로 선택합니다.
따라서 환경 설정 스크립트를 실행하지 않아도 됩니다.

특정 버전을 사용하려면 예제 디렉터리에서 `TOOLCHAIN_ROOT`를 지정합니다.
`X.Y.Z`를 설치된 크로스 툴체인 버전으로 바꾸십시오.

```bash
make clean
make TOOLCHAIN_ROOT=/opt/crosstools/mobilint/1.0.0/vX.Y.Z
```

## 하드웨어 확인

보드에 연결된 V4L2 하드웨어는 다음 명령으로 확인할 수 있습니다.

```bash
v4l2-ctl --list-devices
```

`/dev/videoN`의 번호는 제품 구성에 따라 달라질 수 있습니다.
