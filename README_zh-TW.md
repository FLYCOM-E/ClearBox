![image](https://raw.githubusercontent.com/FLYCOM-E/ClearBox/main/ClearBox.png)

<div align="center">

![✨Clean✨](https://img.shields.io/badge/MagiskModule-blue.svg?logo=android)
[![License](https://img.shields.io/badge/License-GPL--3.0-red.svg?logo=opensourceinitiative)](LICENSE)
[![Downloads](https://img.shields.io/github/downloads/FLYCOM-E/ClearBox/total?color=00D2C4&logo=github)](https://github.com/FLYCOM-E/ClearBox/releases)

[![Telegram](https://img.shields.io/badge/Telegram-blue.svg?logo=telegram)](https://t.me/clearboxmodule)
[![THChat](https://img.shields.io/badge/YHchat-8068E2.svg?logo=icloud&logoColor=white)](https://yhfx.jwznb.com/share?key=yigOTedUjh62&ts=1747355950)

</div>



   # 🌍 ？
> **|** [**简体中文**](https://github.com/FLYCOM-E/ClearBox/blob/main/README.md) **|** [**English**](https://github.com/FLYCOM-E/ClearBox/blob/main/README_en-US.md) **|** **正體中文** **|**



# 什麼是 **ClearBox** ？

**ClearBox** 是一個多功能、深度、快速的清理盒子，可為您的設備一鍵騰空空間、儲存整理、鎖定，軟體快取、安裝包、壓縮包、垃圾、空資料夾，壓制快取、文件歸類、清空，阻止軟體更新/安裝一網打盡，自動優化/清理、文件歸類功能，支持添加自定義清理規則和文件清理/整理歸類、白名單等各種規則，支持利用模組自動化系統添加自定義任務

執行所用時間相對市面上普通清理軟體來說快如✈️，自動化功能僅由一個極低佔用 **Timed** 負責，開銷可直接無視，無軟體後台進程，不強制綁定 App，支持完整功能終端 **TUI**

__兼容 [**Magisk**](https://github.com/topjohnwu/Magisk)、[**APatch**](https://github.com/bmax121/APatch)、[**KernelSU**](https://github.com/tiann/KernelSU) 及其各種分支__



   # 安裝？

   ## [**發行版本**](https://github.com/FLYCOM-E/ClearBox/releases)、[**CI建構**](https://github.com/FLYCOM-E/ClearBox/actions) 或 手動建構：
``` shell
#(請先安裝並設定 NDK，將變數 $NDK 指向 NDK 根目錄)

sudo apt install zip make

git clone https://github.com/FLYCOM-E/ClearBox
cd ClearBox && chmod +x *

export M_API=21 #(可依需求自行調整目標 API 版本)
export M_LANG=zh_TW
export M_TARGET=aarch64 #(可選 aarch64、armv7a、x86_64、i686)

bash build.sh
```
刷入 **ClearBox_aarch64_21_zh_TW.zip**



   # 操作方式：

> ：安裝使用模組 **ClearBox** 軟體（會在模組安裝時詢問

> ：開啟終端 **UI** 功能在任意終端鍵入 **`ClearBox`** 使用



   # 簡介功能：
   
## **1**：一鍵最佳化清理

進行快速一鍵最佳化/清理，效果等同於定期最佳化。包括快取清理、垃圾清理，快速磁碟 **GC** 等，另外根據模組設定還可執行檔案清理/歸類



## **2**：清理/幹掉自訂目錄

這項功能是規則清理功能，可在 **`/data/adb/wipe_cache/CleanConfigs/`** 目錄下自行新增清理規則。

配置方法：

規則使用文字格式，可選在規則檔案開頭第一行填寫 **`@/xxx`** (以根目錄開始) 指定初始目錄，不同規則檔案獨立。

一行一個檔案/目錄路徑，如您已指定初始路徑可以不填寫完整路徑僅填寫相對（初始目錄）路徑即可



## **3**：清除垃圾檔案及空資料夾

清理內/外部儲存空資料夾、多媒體快取、軟體快取等



## **4**：清空所有軟體快取

清理第三方軟體快取，不包括系統軟體快取，如果您開啟了外部儲存快取清理還會清理外部儲存



## **5**：深度檔案清理

檔案清理功能，自帶部分清理配置，支援自訂檔案格式配置，支援外部儲存。

配置方法：

配置目錄： **`/data/adb/wipe-cache/FileConfigs`**

配置方式：在配置檔案目錄新建 **conf** 文件文件，該檔案名稱會顯示在清理頁面當作對應清理項名稱，將自訂檔案副檔名填入該檔案，使用空格或換行分隔，不支援註解。

在檔案第一行填寫 **`@MAX=<size/B/K/M/G>|MIN=<size/B/K/M/G>`** 可以指定目標檔案大小區間

範例：

```
@MAX=1/G|MIN=10/M
```

此宣告指定目標檔案大小區間為：**最大 1 G**，**最小 10 M**

MIN 不得大於 MAX，否則 MIN 自動失效

另：**MAX**、**MIN** 及大小單位是 **不區分大小寫** 的，**暫不支援浮點數（小數）**

軟體/終端清理頁面會根據配置檔案動態建構

範圍：檔案清理，及後面會講到的檔案歸類



## **6**：軟體規則清理

規則清理功能之一，用於根據規則對軟體內部私有目錄進行清理

配置方法：

配置目錄： **`/data/adb/wipe-cache/AppCleanRules`**

配置方式：在配置檔案目錄新建 **conf** 檔案，檔案名稱隨意但不得包含特殊符號、空格等。
第一行開頭填寫 **`@<軟體包名>/<軟體名稱>`** 來指定該規則對應軟體；
之後每行一個資料夾路徑或檔案路徑，初始目錄為該 **App** 的資料目錄。

模組會自動查找該包名對應應用資料目錄，軟體名稱將顯示在清理介面

軟體/終端清理頁面會根據配置檔案動態建構

請不要在規則中以根目錄 **`/xxx`** 硬編碼編寫清理規則，否則將導致清理報錯並跳過此規則檔案。

模組會自動檢查部分配置錯誤



## **7**：清空系統快取

清理系統軟體快取、系統快取、附帶清空 **MTP** 主機資料（可解決 **MTP** 檔案傳輸檔案列表顯示異常問題



## 8：自動清理

功能自動化，由 **Timed** 組件驅動執行（超低開銷）。

### **•  您知道嗎？**

您可以利用此模組 **Timed** 執行自定義任務！請按照以下配置：

配置目錄：**`/data/adb/wipe_cache/TimedConfig`**

在此目錄下建立一個自定義名稱的 **.conf** 檔案，填入並修改以下行（部分可選）：

```
time=間隔時間/時間單位（目前支援 M/分、H/時、D/天）
date=0
run=要執行的程式及參數，支援 Shell 語法（交由 Shell 解析）
in=開始時間/結束時間（在 X 點到 X 點之間執行。這是一個可選參數，填寫值請勿超過 23，注意非天級任務會在此時段內多次執行）
post=通知標題/通知內容（發送通知。這是一個可選參數，不建議在分鐘級任務加入此項）
```

實例：

```
time=1/D
date=0
run=/system/bin/sync
in=0/5
post=Test Run/已執行「/system/bin/sync」
```

以上配置會在每天凌晨 **0~5** 點之間執行一次 **/system/bin/sync** 指令，並發送一條通知。

![image](https://raw.githubusercontent.com/FLYCOM-E/ClearBox/main/Pictures/timed_notification.png)

填寫完後儲存，您需要手動執行 **立即生效當前配置** 或 **重新啟動** 後，該任務才會生效！



## **9**：阻止所有軟體更新安裝

如名，檔案系統級強制阻止軟體安裝，不處理擴充儲存，因此如果軟體是安裝在外部儲存則此項無效



## **10**：內部儲存固定功能

如名，鎖定內部儲存根目錄佈局防止各種軟體亂建立檔案/資料夾，不處理擴充儲存



## **11**：阻止快取功能

這是模組的一個重要功能，用於動態阻止前台軟體產生快取。

開啟該功能後重啟啟用，自動快取前台最近5款軟體並阻止快取，可自訂新增白名單。

名單位置： **`/data/adb/wipe_cache/whitelist.prop`** 或在模組 **ClearBox** 軟體中新增。

**Other**：模組採取動態等待更新的方式（10s ＆ 30s），一方面避免持續資源佔用，一方面部分軟體在阻止快取狀態下啟動會異常，進入軟體後延遲啟動阻止快取是最優解。

阻止快取不會清空該軟體快取，因此軟體快取不會為 **0**，而是緩慢增長，此舉避免不斷進行小檔案擦除而僅僅限制大快取產生

極低開銷，螢幕關閉自動延遲執行

外部儲存擴充僅支援單卡，如果您擴充了多張卡則不會處理擴充儲存

部分軟體出現異常請自行新增白名單！



## **12**：磁鐵（一鍵歸類檔案

一鍵歸類內/多外部儲存檔案，規則與檔案清理共用



## **13**：磁碟 & 軟體最佳化

這是一個最佳化功能集，包含磁碟最佳化（ **f2fs-GC** ）及 **Dexoat** （軟體預編譯）功能


**F2fs** 磁碟 **GC**，此功能屬於緊急 **GC**，因此不需要經常使用，定期執行即可。
方案來自 **Coolapk@Amktiao** 大佬，已得到同意，感謝大佬！


**Dexoat** 軟體預編譯，即提前編譯軟體加速執行時速度，缺點是佔用儲存空間，屬於以空間電量換流暢操作（）
模組預提供了三個模式：系統預設 **Dexoat** 、自訂編譯模式、**RESET**：

系統預設 **Dexoat** ： **Android** 系統（特別是較新系統）也會在空閒時自動編譯部分熱點函數以提高整體執行速度，因此模組提供了系統 **Dexoat** 模式，此模式等於手動觸發系統 **Dexoat**，模式及編譯項由系統決定。

自訂編譯模式：此模式支援手動指定編譯模式，目前支援 **Speed**、**Speed-profile**、**Everything** 三個模式

**RESET**：恢復編譯



## 歡迎以任何形式參與開發、貢獻！

模組 **ClearBox App** 使用 [**Kr-Script**](https://github.com/helloklf/kr-scripts) 項目製作而成，感謝該項目
