# ClearBox에 기여하기

도와주셔서 감사합니다! ClearBox는 주로 한 사람이 유지보수하는 프로젝트입니다(적어도 지금은요 :-) ). 어떤 도움이라도 — 설정 파일 한 줄이라도 — 개발자가 고마워할 겁니다.
아래에서 자신의 강점에 맞는 참여 방식을 골라주세요. **처음 두 항목은 코딩이 필요 없습니다.**



## 기여 방법

| 방향 | 하는 일 | 최종 행선지 |
|---|---|---|
| 설정 | 정리/분류/자동화 규칙을 작성하고 공유하기 | 내장 설정 또는 커뮤니티 규칙 모음 |
| 번역 | 다국어 번역 문자열 개선하기 | `.docs/README_*.md` 또는 앱 / TUI 내 텍스트 |
| 테스트 | 자신의 기기/ROM에서 빌드하고 버그 보고하기 | Issues 또는 Telegram 그룹 |
| 코드 | C 코어, Kr-Script 앱, 빌드 스크립트 개발 참여 | Pull Requests & Push |



### 설정 (규칙 작성과 디버깅)

아래는 전부 순수 텍스트입니다 — `.txt`를 편집할 수만 있으면 기여할 수 있습니다:

프리셋 설정:
- `AppSource/clearbox/src/main/assets/usr/Timed_xml/` — 자동화 작업
- `FileConfigs/` — 파일 정리 / 분류 규칙
- `AppConfigs/` — 앱 비공개 저장소 규칙

커스텀 설정:
- `/data/adb/wipe_cache/CleanConfigs` — 어질러진 디렉터리 정리 규칙
- `/data/adb/wipe_cache/BindConfigs` — 저장소 리디렉션 규칙

문법 참고: README의 각 섹션 설명을 확인하세요. 지켜야 할 규칙:

- PR(또는 파일) 하나 = 규칙 파일 하나, 정리 대상의 이름으로 명명
- 일부 규칙은 상대 경로를 사용하므로, 루트 `/xxx`로 시작하는 경로를 **하드코딩하지 마세요**
- `#` 주석은 되도록 한 줄을 온전히 차지하게 하세요(행 끝의 `#ext` 인라인 주석이 유일한 예외)
- 자신의 기기에서 규칙을 테스트하고, PR에 테스트 기기 / ROM / Android 버전을 함께 적어주세요.
- 프리셋 설정은 위 프리셋 디렉터리에 커밋하고 **PR을 열어주세요**
- 커스텀 설정은 커뮤니티에 공유하거나 Telegram 그룹에서 공유하는 것을 권장합니다



### 번역 (문자열 다듬기)

오타, 어색한 표현, 또는 번역되지 않은 문자열을 찾으셨나요? PR 환영합니다.
빠른 리뷰를 위해 PR마다 **한 언어만** 수정해 주세요.
프로젝트에는 현재 다음 언어가 있으며, 자신 있는 언어를 고르시면 됩니다:
- **`en-US / zh-CN / zh-TW / ru-RU / ko-KR / ja-JP`**
(어, 못 찾으셨나요 :-)? 그럼 그때는 부탁드릴 수밖에요)



### 테스트 (기능 개선 및 버그 보고)

문제를 보고할 때는 다음을 포함해 주세요:

- 기기 및 Android 버전:
- ROM (MIUI / HyperOS / AOSP...)
- Root 방식: Magisk / APatch / KernelSU + 버전
- ClearBox 버전 (Version Code)
- 파일 시스템(안다면): f2fs / ext4 / sdcardfs
- 실제 발생한 상황 vs 기대한 동작
- 터미널 출력 또는 로그(있다면)



### 코드 (기능 추가 / 버그 수정)

- **NDK**를 설정하고 환경 변수 **ANDROID_NDK_HOME**을 **NDK** 루트 디렉터리 경로로 지정해 주세요
- **App** 코드 개발과 디버깅에 깊이 참여한다면 **Android SDK** 환경도 설정해 주세요
- 기본 환경 설정 몇 가지는 다음과 같습니다:

기본 패키지 도구

``` shell
# Debian, Ubuntu 계열
sudo apt install zip make build-essential

# Arch 계열
sudo pacman -S zip make base-devel

# Alpine 계열
sudo apk add zip make build-base

# Fedora 계열
sudo dnf install zip make
sudo dnf groupinstall "Development Tools"
```

프로젝트 가져오기(참고용 — PR을 낼 거라면 직접 Fork하세요)

``` shell
git clone https://github.com/FLYCOM-E/ClearBox
cd ClearBox/ && chmod +x *

export TARGET_API=23         # 필요에 따라 타겟 api 버전을 조정
export TARGET_ABI=aarch64   # 선택 가능: aarch64, armv7a, x86_64, riscv64
```

코어 컴파일

``` shell
./build.sh       # APK 컴파일이 필요하면: --build-apk
```

커밋 규칙: 기존 스타일을 유지하세요: `Fix: ...` / `Update: ...` / `Add: ...`, 커밋 하나는 가급적 하나의 관련 변경만 하고, 변경이 여러 개면 줄바꿈으로 구분해 주세요.
PR 요구사항: PR 하나 = 기능 하나 또는 수정 하나. 무엇을 변경했는지, **왜** 변경했는지 설명해 주세요(필요하다면. 실제로 겪은 실패 사례가 가장 좋은 "왜"입니다).



## 🚫 하지 말아 주세요

- 위의 기기 정보 없이 "안 돼요" 식의 issue를 올리지 마세요. 정보가 없으면 메인테이너가 재현조차 못 할 수 있습니다
- 기능 아이디어: 많은 코드를 작성하기 전에 먼저 Issue를 만들어 논의해 주세요. 시간 낭비를 막기 위함입니다
- 같은 커밋에 서식 변경과 로직 변경을 섞지 마세요
- 테스트되지 않은 정규식 추측식 규칙을 커밋하지 마세요



## ✍️ 논의 채널

- Issues: **버그** 및 기능 추적용
- Telegram(가장 활발함): [**Group**](https://t.me/clearbox_group)



## ❤️ 감사의 말

이 프로젝트는 [GPL-3.0](https://github.com/FLYCOM-E/ClearBox/blob/main/LICENSE) 라이선스로 배포됩니다 ~ 기여하는 순간, 귀하의 성과가 동일한 라이선스로 배포되는 데 동의하게 됩니다.
기여자 명단이 이곳에 표시됩니다. 여러분의 이름이 이 목록의 맨 앞에 있을지도 모릅니다.



### 다음 기여자분들께 감사드립니다:

[**Amktiao**](https://www.coolapk.com/u/925348): F2FS-GC 원리를 간접적으로 제공
[**Drsexo**](https://github.com/Drsexo): Github Action 설정 추가
[**Creeper005bili**](https://github.com/Creeper005bili): 모듈 초기 테스트에 도움
