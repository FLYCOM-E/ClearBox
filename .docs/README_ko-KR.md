![image](https://raw.githubusercontent.com/FLYCOM-E/ClearBox/main/ClearBox.png)

<div align="center">

![✨Clean✨](https://img.shields.io/badge/MagiskModule-blue.svg?logo=android)
[![License](https://img.shields.io/badge/License-GPL--3.0-red.svg?logo=opensourceinitiative)](LICENSE)
[![Downloads](https://img.shields.io/github/downloads/FLYCOM-E/ClearBox/total?color=00D2C4&logo=github)](https://github.com/FLYCOM-E/ClearBox/releases)

[![Telegram](https://img.shields.io/badge/Telegram-blue.svg?logo=telegram)](https://t.me/clearbox_update)
[![THChat](https://img.shields.io/badge/YHchat-8068E2.svg?logo=icloud&logoColor=white)](https://yhfx.jwznb.com/share?key=yigOTedUjh62&ts=1747355950)

</div>



# 🌍 ？

> **|** [**English**](https://github.com/FLYCOM-E/ClearBox/blob/main/README.md) **|** [**简体中文**](https://github.com/FLYCOM-E/ClearBox/blob/main/.docs/README_zh-CN.md) **|** [**正體中文**](https://github.com/FLYCOM-E/ClearBox/blob/main/.docs/README_zh-TW.md) **|** [**Русский**](https://github.com/FLYCOM-E/ClearBox/blob/main/.docs/README_ru-RU.md) **|** **한국어** **|** [**日本語**](https://github.com/FLYCOM-E/ClearBox/blob/main/.docs/README_ja-JP.md) **|**



# ClearBox란 무엇인가요？

**ClearBox**는 다기능, 심층적이고 빠른 클리닝 박스로, 기기에서 한 번의 클릭으로 공간을 확보하고, 저장소를 정리 및 잠그며, 앱 캐시, 설치 패키지, 압축 파일, 쓰레기, 빈 폴더를 정리하고, 캐시 생성 억제, 파일 분류 및 비우기, 앱 업데이트/설치 차단 등을 모두 처리할 수 있습니다. 사용자 정의 정리 규칙, 파일 정리/분류 규칙, 화이트리스트 등 다양한 규칙을 추가할 수 있으며, 모듈의 자동화 시스템을 활용하여 사용자 정의 작업을 추가할 수도 있습니다.

실행 시간은 일반적인 클리닝 앱에 비해 ✈️처럼 빠르며, 자동화 기능은 극도로 낮은 점유율의 **Timed** 구성 요소 하나만으로 작동하므로 오버헤드는 무시할 수 있습니다. 백그라운드 프로세스가 없고, 앱을 강제로 바인딩하지 않으며, 완전한 기능의 터미널 **TUI**를 지원합니다.

![image](https://raw.githubusercontent.com/FLYCOM-E/ClearBox/main/.Images/app_pages.png)

__[**Magisk**](https://github.com/topjohnwu/Magisk), [**APatch**](https://github.com/bmax121/APatch), [**KernelSU**](https://github.com/tiann/KernelSU) 및 다양한 포크와 호환됩니다.__



# 설치 방법？

### [**릴리스 버전**](https://github.com/FLYCOM-E/ClearBox/releases)

### [**CI 빌드**](https://github.com/FLYCOM-E/ClearBox/actions/workflows/building_modules.yml)



# 사용 방법：

> ：모듈 ClearBox 앱을 설치하여 사용합니다 (모듈 설치 시 묻습니다)

> ：터미널 UI 기능을 켜고 아무 터미널에서 **`ClearBox`**를 입력하여 사용합니다



# 기능 소개：


   
## 원클릭 최적화 및 정리

빠른 원클릭 최적화/정리를 수행하며, 정기 최적화와 동일한 효과를 제공합니다. 캐시 정리, 쓰레기 정리, 빠른 디스크 **GC** 등을 포함하며, 모듈 설정에 따라 파일 정리/분류도 실행할 수 있습니다.



## 사용자 정의 규칙 정리

이 기능은 규칙 기반 정리 기능으로, **`/data/adb/wipe_cache/CleanConfigs`** 디렉터리에 직접 정리 규칙을 추가할 수 있습니다.

구성 방법：

규칙은 텍스트 형식을 사용하며, 규칙 파일의 첫 번째 줄에 **`@/xxx`** (루트 디렉터리부터 시작)를 입력하여 초기 디렉터리를 지정할 수 있습니다. 각 규칙 파일은 독립적으로 작동합니다.

한 줄에 하나의 파일/디렉터리 경로를 입력합니다. 초기 경로를 지정한 경우 전체 경로 대신 상대 경로(초기 디렉터리 기준)만 입력해도 됩니다.



## 쓰레기 파일 및 빈 폴더 정리

내부/외부 저장소의 빈 폴더, 미디어 캐시, 앱 캐시 등을 정리합니다.



## 모든 앱 캐시 비우기

시스템 앱 캐시를 제외한 서드파티 앱 캐시를 정리합니다. 외부 저장소 캐시 정리가 활성화된 경우 외부 저장소도 정리됩니다.



## 심층 파일 정리

파일 정리 기능으로, 기본 제공되는 일부 정리 구성을 포함하며, 사용자 정의 파일 형식 구성 및 외부 저장소를 지원합니다.

구성 방법：

구성 디렉터리： **`/data/adb/wipe_cache/FileConfigs`**

구성 방식： 구성 파일 디렉터리에 **.conf** 문서 파일을 새로 만듭니다. 파일 이름(확장자 제외)은 정리 페이지에 해당 정리 항목의 이름으로 표시됩니다. 사용자 정의 파일 확장자를 이 파일에 입력하고, 공백 또는 줄바꿈으로 구분합니다. 행 앞에 `#`을 붙이면 주석이 됩니다.

파일 내에 **`@MAX 또는 MIN = < 크기 / B 또는 BYTE / K 또는 KB / M 또는 MB / G 또는 GB >`**를 입력하여 대상 파일 크기 범위를 지정할 수 있습니다. 후행 선언이 이전 선언을 덮어쓰므로 이 기능을 활용하여 원하는 규칙을 작성할 수 있습니다.

예시：

``` conf
# 이것은 주석입니다

# 이 선언은 최대 1G, 최소 10MB의 zip 및 7z 파일만 분류함을 나타냅니다
# 그러나 iso 및 img는 최대 크기 제한이 없습니다

@MAX=1/G @MIN=10/M

zip 7z

# max -1은 제한 없음을 의미합니다 (max는 다음 선언까지 적용되지 않지만 min은 적용됨)

@max=-1
iso img

# KB, MB, GB도 지원됩니다 &

@MIN=10/KB exe
@MIN=1/MB apk
```

참고：**MIN**은 **MAX**보다 클 수 없으며, 그렇지 않으면 **MIN**이 자동으로 무시됩니다. 더 큰 **MIN**을 설정하기 전에 **MAX**를 재설정하세요. **MAX**、**MIN** 및 크기 단위는 **대소문자를 구분하지 않습니다**（이미 눈치채셨겠죠 :⁠-⁠)）。**소수점(실수)은 지원되지 않습니다**.

앱/터미널 정리 페이지는 구성 파일에 따라 동적으로 구축됩니다.

적용 범위： 파일 정리, 파일 분류.



## 앱 규칙 정리

규칙 기반 정리 기능 중 하나로, 앱의 내부 개인 디렉터리를 규칙에 따라 정리합니다.

구성 방법：

구성 디렉터리： **`/data/adb/wipe_cache/AppCleanRules`**

구성 방식： 구성 파일 디렉터리에 **conf** 파일을 새로 만듭니다. 파일 이름은 자유롭게 지정할 수 있지만 특수 기호나 공백은 포함할 수 없습니다.
첫 번째 줄에 **`@<앱 패키지명>/<앱 이름>`**을 입력하여 해당 규칙이 적용될 앱을 지정합니다.
이후 각 줄에 폴더 경로 또는 파일 경로를 입력합니다. 초기 디렉터리는 해당 **App**의 데이터 디렉터리입니다.

모듈은 지정된 패키지명에 해당하는 앱 데이터 디렉터리를 자동으로 찾으며, 앱 이름은 정리 인터페이스에 표시됩니다.

앱/터미널 정리 페이지는 구성 파일에 따라 동적으로 구축됩니다.

루트 디렉터리 **`/xxx`**로 시작하는 절대 경로를 규칙에 하드코딩하지 마세요. 그렇지 않으면 정리 오류가 발생하고 이 규칙 파일은 건너뜁니다.

모듈은 일부 구성 오류를 자동으로 확인합니다.



## 시스템 캐시 비우기

시스템 앱 캐시, 시스템 캐시를 정리하고 **MTP** 호스트 데이터도 함께 비웁니다 (**MTP** 파일 전송 시 파일 목록 표시 오류를 해결할 수 있습니다).



## [Beta] 저장소 분석 (NCDU)

저장소 디렉터리 사용량을 분석합니다. 누가 내 저장소 공간을 잡아먹었을까요?



## 자동 정리

기능 자동화는 **Timed** 구성 요소에 의해 구동되며 초저전력으로 작동합니다.

### **•  알고 계셨나요？**

이 모듈의 **Timed**를 활용하여 사용자 정의 작업을 실행할 수 있습니다！다음과 같이 구성하세요：

구성 디렉터리：**`/data/adb/wipe_cache/TimedConfigs`**

이 디렉터리에 임의의 이름을 가진 **.conf** 파일을 생성하고 다음 줄을 입력하여 수정합니다 (일부는 선택 사항)：

``` conf
time=간격 시간/시간 단위 (현재 지원: M/분, H/시, D/일)
date=마지막 실행 시간 (타임스탬프 숫자). 선택적 매개변수로, 입력하지 않으면 프로그램이 자동으로 현재 시간으로 설정합니다. -1로 설정하면 즉시 실행됩니다.
run=실행할 프로그램 및 매개변수, Shell 문법 지원 (Shell에서 구문 분석)
in=시작 시간/종료 시간 (X시에서 Y시 사이에만 실행. 선택적 매개변수)
post=알림 제목/알림 내용 (알림 전송. 선택적 매개변수, 분 단위 작업에는 권장하지 않음)
```

**time**, **run**은 필수 항목이며, 모든 **KEY**는 대소문자를 구분하지 않습니다.
예시：

``` conf
time=1/D
date=0
run=sync
in=0/5
post=Test Run/Sync 실행됨
```

위 구성은 매일 새벽 **0~5**시 사이에 **`/system/bin/sync`** 명령을 한 번 실행하고 알림을 하나 전송합니다.

![image](https://raw.githubusercontent.com/FLYCOM-E/ClearBox/main/.Images/timed_notification.png)



## 모든 앱 업데이트/설치 차단

이름 그대로, 파일 시스템 수준에서 앱 설치를 강제로 차단합니다. 확장 저장소는 처리하지 않으므로, 앱이 외부 저장소에 설치된 경우 이 기능은 적용되지 않습니다.



## 내부 저장소 고정 기능

이름 그대로, 내부 저장소 루트 디렉터리 레이아웃을 잠가 각종 앱이 파일/폴더를 무단으로 생성하는 것을 방지합니다. 확장 저장소는 처리하지 않습니다.



## 캐시 생성 차단 기능

이 모듈의 중요한 기능으로, 포그라운드 앱의 캐시 생성을 동적으로 차단합니다.

이 기능을 켠 후 재부팅하면 활성화되며, 최근 포그라운드 앱 5개를 자동으로 캐시하고 캐시를 차단합니다. 화이트리스트를 사용자 정의하여 추가할 수 있습니다.

화이트리스트 위치： **`/data/adb/wipe_cache/whitelist.prop`** 또는 모듈 **ClearBox** 앱에서 추가할 수 있습니다.

**기타**：모듈은 동적 대기 업데이트 방식(10초 및 30초)을 사용하여 지속적인 리소스 점유를 방지하고, 일부 앱은 캐시 차단 상태에서 실행 시 오류가 발생할 수 있으므로 앱 진입 후 지연 시작하여 캐시를 차단하는 것이 최적의 해결책입니다.

캐시 차단은 해당 앱의 캐시를 비우지 않으므로 앱 캐시가 **0**이 되지 않고 서서히 증가합니다. 이는 지속적인 소규모 파일 삭제를 피하고 대용량 캐시 생성만 제한하기 위함입니다.

초저전력 소모, 화면 꺼짐 시 자동 지연 실행

일부 앱에서 이상이 발생하면 직접 화이트리스트에 추가하세요！



## 자석 (원클릭 파일 분류)

내부 및 여러 외부 저장소의 파일을 한 번에 분류합니다. 규칙은 파일 정리와 공유하며, 사용자 정의 분류 디렉터리를 지원합니다.



## 규칙 디렉터리 바인딩 (리디렉션)

구성 디렉터리：**`/data/adb/wipe_cache/BindConfigs`**

이 디렉터리에 임의의 이름을 가진 **.conf** 파일을 생성하고 사용자 정의 마운트 규칙을 입력하세요：

``` conf
<바인딩 디렉터리> <대상 디렉터리>
```

참고：**절대 경로**가 아닌 **내부 저장소 기준 상대 경로**를 입력하세요. 그렇지 않으면 마운트에 실패합니다.

**<바인딩 디렉터리>** 내에 디렉터리/파일을 쓰거나 생성하면 **<대상 디렉터리>**로 리디렉션됩니다.

이 기능은 **sdcardfs**를 사용하는 기기에서는 현재 실행할 수 없지만, **Android 12+**에서는 대부분 sdcardfs를 사용하지 않으므로 걱정하지 않으셔도 됩니다.



## 디스크 및 앱 최적화

디스크 최적화(**f2fs-GC**) 및 **Dexoat**(앱 사전 컴파일) 기능을 포함하는 최적화 기능 모음입니다.


**F2fs** 디스크 **GC**는 긴급 **GC**에 해당하므로 자주 사용할 필요는 없으며 정기적으로 실행하면 됩니다.
이 솔루션은 **Coolapk@Amktiao** 님의 작업을 기반으로 하며, 허가를 받았습니다. 감사합니다！

![image](https://raw.githubusercontent.com/FLYCOM-E/ClearBox/main/.Images/f2fs_gc.png)

**Dexoat** 앱 사전 컴파일은 앱을 미리 컴파일하여 실행 속도를 가속화하지만, 저장 공간을 더 많이 사용하는 단점이 있습니다 (공간과 배터리를 희생하여 부드러운 작동을 얻는 방식입니다).
모듈은 세 가지 모드를 기본 제공합니다：시스템 기본 **Dexoat**, 사용자 정의 컴파일 모드, **RESET**：

시스템 기본 **Dexoat** ： **Android** 시스템(특히 최신 버전)도 유휴 시 일부 핫스팟 함수를 자동으로 컴파일하여 전체 실행 속도를 향상시킵니다. 따라서 모듈은 시스템 **Dexoat** 모드를 제공하며, 이는 수동으로 시스템 **Dexoat**를 트리거하는 것과 동일하며, 모드 및 컴파일 항목은 시스템에 의해 결정됩니다.

사용자 정의 컴파일 모드：이 모드는 컴파일 모드를 수동으로 지정할 수 있으며, 현재 **Speed**, **Speed-profile**, **Everything** 세 가지 모드를 지원합니다.

**RESET**：컴파일 상태 복원



## 모든 형태의 개발 및 기여를 환영합니다！

모듈 기능에 대한 의견 & 제안을 환영합니다. 감사합니다.

[**👉 개발 가이드**](https://github.com/FLYCOM-E/ClearBox/blob/main/.docs/dev/CONTRIBUTING_ko-KR.md)

모듈 **ClearBox App**은 [**Kr-Script**](https://github.com/helloklf/kr-scripts) 프로젝트를 기반으로 제작되었으며, 이 프로젝트에 감사드립니다.
