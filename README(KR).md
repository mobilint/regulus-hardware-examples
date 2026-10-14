# Regulus 하드웨어 예제

[English](README.md)

<!-- markdownlint-disable MD033 -->
<div align="center">
<p>
<a href="https://www.mobilint.com/" target="_blank">
<img src="./assets/Mobilint_Logo.png" alt="Mobilint Logo" width="60%">
</a>
</p>
</div>
<!-- markdownlint-enable MD033 -->

이 저장소는 Mobilint Regulus SoC에 내장된 하드웨어를 쉽게 사용할 수 있도록 C++ 예제와 설명을 제공합니다.

각 예제에서는 이미지 및 동영상 데이터를 하드웨어로 처리하고, 결과를 파일로 저장하여 확인하는 과정을 안내합니다.

## 시작하기 전에

예제를 실행하려면 Regulus 보드와 해당 보드용 SDK 및 크로스 툴체인이 필요합니다.

SDK는 [Mobilint 다운로드 센터](https://dl.mobilint.com/)에서 받을 수 있습니다.

제품별 환경 설정, 빌드, 보드 전송 및 실행 방법은 다음 문서를 참조하세요.

- [Regulus-RA 하드웨어 예제](regulus-ra/README(KR).md)
- [Regulus-RB 하드웨어 예제](regulus-rb/README(KR).md)

## 지원 보드

이 저장소는 다음 Regulus 보드를 지원합니다.

- **Regulus-RA**
- **Regulus-RB**

보드마다 제공되는 하드웨어와 크로스 툴체인이 다릅니다. 사용하는 보드의 디렉터리에서 예제를 진행하십시오.

## 개요

| 하드웨어 | 역할 | Regulus-RA | Regulus-RB |
| --- | --- | :---: | :---: |
| MAPI | 이미지 크기와 RGB/YUV 픽셀 형식을 변환합니다. | [예제](regulus-ra/mapi/README(KR).md) | [예제](regulus-rb/mapi/README(KR).md) |
| CODAJ | JPEG 이미지를 압축하거나 압축 해제합니다. | [예제](regulus-ra/codaj/README(KR).md) | [예제](regulus-rb/codaj/README(KR).md) |
| DMA350 | 메모리를 복사하고 이미지를 자르거나 회전·반전합니다. | - | [예제](regulus-rb/dma350/README(KR).md) |
| ISP | MIPI 카메라의 RAW 영상을 처리하고 캡처합니다. | [예제](regulus-ra/isp/README(KR).md) | [예제](regulus-rb/isp/README(KR).md) |
| Wave5 | H.264/H.265 영상을 압축하거나 압축 해제합니다. | [예제](regulus-ra/wave5/README(KR).md) | [예제](regulus-rb/wave5/README(KR).md) |
| SP500 | 영상 및 신호 처리 연산을 실행하는 Vision DSP입니다. | - | [예제](regulus-rb/sp500/README(KR).md) |

표의 `-`는 이 저장소에서 해당 보드용 예제를 제공하지 않음을 뜻합니다.

각 디렉터리의 `README(KR).md`에는 하드웨어의 역할, 예제 데이터 흐름, 빌드 방법 및 실행 방법이 설명되어 있습니다.

## 사용 순서

1. 사용하는 보드의 디렉터리로 이동합니다.
2. 보드용 크로스 툴체인을 준비합니다.
3. 실행하려는 하드웨어 예제 디렉터리에서 `make`를 실행합니다.
4. 예제 디렉터리를 보드로 전송합니다.
5. 보드에서 예제를 실행하고 `outputs/`에 생성된 결과를 확인합니다.

자세한 명령은 각 예제의 `README(KR).md`를 참조하세요.

## 저장소 구조

```text
regulus-hardware-examples-gitlab/
├── assets/             # 문서에 사용되는 이미지
├── regulus-ra/         # Regulus-RA 하드웨어 예제
│   ├── codaj/          # JPEG 압축 및 압축 해제
│   ├── isp/            # MIPI 카메라 ISP 영상 캡처
│   ├── mapi/           # 이미지 크기 및 픽셀 형식 변환
│   └── wave5/          # H.264/H.265 압축 및 압축 해제
└── regulus-rb/         # Regulus-RB 하드웨어 예제
    ├── codaj/          # JPEG 압축 및 압축 해제
    ├── dma350/         # 메모리 복사 및 이미지 배치 변환
    ├── isp/            # MIPI 카메라 ISP 영상 캡처
    ├── mapi/           # 이미지 크기 및 픽셀 형식 변환
    ├── sp500/          # SP500 letterbox 전처리
    └── wave5/          # H.264/H.265 압축 및 압축 해제
```

## 지원 및 이슈

예제를 진행하는 동안 문제가 발생하면 [기술 지원 이메일](mailto:tech-support@mobilint.com)로 문의하세요.
