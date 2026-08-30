# Version 5.1.5 (98)

# ADD
- NCDU 歷史自動清理
- 支援日語
- 支援韓語

# UPDATE
- NCDU 歷史功能改為 SQlite3 儲存資料
- 最佳化 App 桌布模式
- 最佳化多語言匹配，減少重複邏輯

# FIX
- 部分裝置 `settings get system system_locales` 回傳 null 導致語言回退問題，現已優先回退至系統預設值
- 修復一些記憶體洩漏問題
