# 為 ClearBox 貢獻

感謝您願意提供協助！ClearBox 是主力由單人維護的專案（至少目前 :-) ），因此任何幫助——哪怕只是一行設定檔——開發者都會感謝您。
請依您的專長選擇下方的參與方式。**前兩項無需撰寫程式碼。**



## 貢獻方式

| 方向 | 工作內容 | 最終去向 |
|---|---|---|
| 設定 | 編寫並分享您的清理/整理/自動化規則 | 內建設定或社群規則集 |
| 翻譯 | 改進多語言翻譯字串 | `.docs/README_*.md` 或應用程式 / TUI 內文字 |
| 測試 | 在您的裝置/ROM 上執行建置，回報 Bug | Issues 或 Telegram 群組 |
| 程式碼 | 參與 C 核心、Kr-Script 應用程式、建置腳本開發 | Pull Requests & Push |



### 設定（設定編寫與除錯）

以下全部是純文字——只要您會編輯 `.txt`，就能貢獻：

預設設定：
- `AppSource/clearbox/src/main/assets/usr/Timed_xml/` — 自動化任務
- `FileConfigs/` — 檔案清理 / 歸類規則
- `AppConfigs/` — 應用程式私有儲存規則

自訂設定：
- `/data/adb/wipe_cache/CleanConfigs` — 雜亂目錄清理規則
- `/data/adb/wipe_cache/BindConfigs` — 儲存重定向規則

語法參考：請查看 README 中各部分的說明。需遵守的規則：

- 一個 PR（或一個檔案）對應一個規則檔，以所清理的內容命名
- 部分規則使用相對路徑，**請不要** 編寫以根目錄 `/xxx` 開始的路徑
- `#` 註解盡量獨佔一行（行尾的 `#ext` 內聯註解是唯一的例外）
- 請在您自己的裝置上測試規則，PR 並附測試裝置 / ROM / Android 版本。
- 預設設定請提交至以上預設目錄並 **發起 PR**
- 自訂設定建議您分享至社群或在 Telegram 群組中



### 翻譯（完善字串）

發現拼寫錯誤、表達生硬或未翻譯的字串？歡迎 PR。
請盡量每個 PR **只修改一種語言**，以便快速審閱。
專案目前包含以下語言，您可以選擇您擅長的：
- **`en-US / zh-CN / zh-TW / ru-RU / ko-KR / ja-JP`**
（怎麼，您發現沒有 :-)？那只好麻煩您一下了）



### 測試（功能改善與回報）

回報問題時，請包含：

- 裝置與 Android 版本：
- ROM（MIUI / HyperOS / AOSP...）
- Root 方案：Magisk / APatch / KernelSU + 版本號
- ClearBox 版本（Version Code）
- 檔案系統（如果知道）：f2fs / ext4 / sdcardfs
- 實際發生什麼 vs 預期行為
- 終端輸出或日誌（如有）



### 程式碼（功能新增 / Bug 修復）

- 請設定 **NDK** 並將環境變數 **ANDROID_NDK_HOME** 設為 **NDK** 根目錄路徑
- 如果深度參與 **App** 程式碼開發與除錯請設定 **Android SDK** 環境
- 以下是一些基礎環境設定：

基礎套件工具

``` shell
# Debian、Ubuntu 系
sudo apt install zip make build-essential

# Arch 系
sudo pacman -S zip make base-devel

# Alpine 系
sudo apk add zip make build-base

# Fedora 系
sudo dnf install zip make
sudo dnf groupinstall "Development Tools"
```

拉取專案（僅供參考，如果 PR 請自行 Fork）

``` shell
git clone https://github.com/FLYCOM-E/ClearBox
cd ClearBox/ && chmod +x *

export TARGET_API=23         # 可以自行根據需要調整目標 api 版本
export TARGET_ABI=aarch64   # 目前可選: aarch64、armv7a、x86_64、riscv64
```

編譯核心

``` shell
./build.sh       # 如需編譯 APK: --build-apk
```

提交規範：保持現有風格: `Fix: ...` / `Update: ...` / `Add: ...`，盡量每個提交只做一項相關更改，如果有多項更改請添加換行。
PR 要求：一個 PR = 一個功能或一個修復。請描述您更改了什麼以及 **為什麼**（如有必要。您遇到的實際失敗案例是最好的「為什麼」）。



## 🚫 請不要

- 請不要在沒有上述裝置資訊的情況下提交「它不工作」的 issue，沒有這些資訊維護者可能根本無法重現
- 功能創意：在編寫大量程式碼之前，請先建立一個 Issue 進行討論，以免浪費您的時間
- 不要將格式調整與邏輯更改混在同一個提交中
- 不要提交未經測試的正則猜測式規則



## ✍️ 討論管道

- Issues：用於 **Bug** 和功能追蹤
- Telegram（最活躍）：[**Group**](https://t.me/clearbox_group)



## ❤️ 致謝

本專案的授權為 [GPL-3.0](https://github.com/FLYCOM-E/ClearBox/blob/main/LICENSE) ~ 一旦您貢獻，即表示您同意您的成果以相同授權散佈。
貢獻者將在此列出。您的名字可能成為這個清單中靠前的一個。



### 感謝以下貢獻者：

[**Amktiao**](https://www.coolapk.com/u/925348): 間接提供 F2FS-GC 原理

[**Drsexo**](https://github.com/Drsexo): 添加 Github Action 設定

[**Creeper005bili**](https://github.com/Creeper005bili): 在模組早期幫助測試
