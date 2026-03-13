# Version Code 87

## 新增
- Timed 新增時間範圍功能，在配置中新增 in 欄位，填寫方式如下：

```
in=開始時間/結束時間（在 X 點到 X 點之間執行。這是一個可選參數，填寫值請勿超過 23，注意非天級任務可能會在此時段內多次執行）
```

實例：

```
time=1/D
date=0
run=/system/bin/sync
in=0/5
post=Test Run/已執行「/system/bin/sync」
```

以上配置會在每天凌晨 0~5 點之間執行一次 **`/system/bin/sync`** 指令，並發送一條通知。

- 一鍵清理增加清理項目：App 規則清理，一鍵清理所有 App。
- 測試上線部分 App 清理規則，模組會自動處理以下對應關係：

```
Google Play  ： com.android.vending
Google 相簿  ： com.google.android.apps.photos
GMS          ： com.google.android.gms
Google 輸入法 ： com.google.android.inputmethod.latin
YouTube      ： com.google.android.youtube
Edge         ： com.microsoft.emmx
Outlook      ： com.microsoft.office.outlook
雲湖          ： com.yhchat.app
拼多多商家版   ： com.xunmeng.merchant
```

## 更新
- Timed 配置執行指令支援 Shell 語法，新增分鐘對齊。
- 修復 App 規則清理的部分 Bug。
- 修復配置備份指令檔刪除記錄檔名稱錯誤的 Bug。
- 修復終端機 TUI 模式下無法看到清理統計資訊的問題。
- 優化 Shell 指令檔。
