# Regulus-RB SP500 Example

[English](README.md)

## SP500 Vision DSP

SP500은 Regulus-RB의 Vision DSP입니다.
SP500은 영상 및 신호 처리 연산을 CA53 호스트 대신 수행합니다.
CA53 호스트 응용 프로그램은 `libsp500`을 사용하여 입력 버퍼와 연산 명령을 DSP에 전달합니다.
SP500 펌웨어는 FreeRTOS에서 실행되며 호스트의 연산 요청을 처리합니다.
공개 API는 letterbox, log-mel spectrogram, YOLO 디코딩 및 후처리를 제공합니다.
주요 DSP 연산 API는 상태 코드를 반환합니다. 호스트 응용 프로그램에서 이 코드로 연산 결과를 확인할 수 있습니다.

## 예제

이 예제는 SP500 DSP에서 입력 이미지를 640x640 크기로 letterbox 처리합니다.
Letterbox는 원본의 가로세로 비율을 유지한 상태로 이미지 크기를 변경합니다.
남는 영역은 회색 여백으로 채웁니다.

처리 순서는 다음과 같습니다.

1. OpenCV로 `test-sample-1080p.png`를 읽습니다.
2. `libsp500`으로 DSP에 연결하고 공유 출력 버퍼를 할당합니다.
3. SP500에서 letterbox 연산을 실행합니다.
4. 결과를 `outputs/letterbox_out.png`로 저장합니다.

## 빌드

개발 PC의 저장소 최상위 디렉터리에서 다음 명령을 실행합니다.
`make`는 `/opt/crosstools/mobilint/2.0.0` 아래에서 환경 설정 파일이 있는 최신 유효 툴체인을 기본으로 선택합니다.

```bash
cd regulus-rb/sp500
make
```

특정 크로스 툴체인을 사용하려면 `TOOLCHAIN_ROOT`를 지정합니다.
`X.Y.Z`를 설치한 크로스 툴체인 버전으로 바꾸십시오.

```bash
make clean
make TOOLCHAIN_ROOT=/opt/crosstools/mobilint/2.0.0/vX.Y.Z
```

빌드가 완료되면 ARM64 실행 파일 `sp500_letterbox`가 생성됩니다.

## 보드로 전송

`<BOARD_IP>`를 보드 IP 주소로 바꾸십시오.
개발 PC의 저장소 최상위 디렉터리에서 다음 명령을 실행합니다.

```bash
cd regulus-rb/sp500
ssh root@<BOARD_IP> 'mkdir -p /root/sp500'
scp sp500_letterbox test-sample-1080p.png root@<BOARD_IP>:/root/sp500/
```

## DSP 펌웨어 로드

SP500 프로그램을 실행하려면 DSP 펌웨어가 필요합니다.
보드의 전원을 켜거나 보드를 재부팅한 후 다음 명령을 한 번 실행하십시오.

```bash
ssh root@<BOARD_IP>
dsp_firmware_load
```

펌웨어 로드 중에는 다음과 유사한 메시지가 표시됩니다. 메시지는 SDK 버전에 따라 다를 수 있습니다.

```text
SP500 reset assert
SP500 ptcm     loading complete: <BYTES> bytes from /usr/lib/firmware/mobilint/sp500/ptcm.bin
SP500 dtcm     loading complete: <BYTES> bytes from /usr/lib/firmware/mobilint/sp500/dtcm.bin
SP500 external loading complete: <BYTES> bytes from /usr/lib/firmware/mobilint/sp500/external.bin
SP500 reset de-assert; DSP running
```

오류 메시지 없이 로드가 완료된 후 SP500 프로그램을 실행하십시오.

## 실행

```bash
cd /root/sp500
./sp500_letterbox test-sample-1080p.png
```

정상 실행 후 `outputs/letterbox_out.png` 파일이 생성됩니다.

## 추가 예제

더 많은 전처리와 후처리 예제는 [regulus-dsp-examples](https://github.com/mobilint/regulus-dsp-examples)를 참고하십시오.
