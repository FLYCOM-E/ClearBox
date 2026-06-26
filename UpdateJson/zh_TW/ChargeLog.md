# Version 5.1.2 (94)

# ADD
- 檔案歸類支援設定歸類目錄，支援填寫絕對/相對路徑
- 系統快取清理新增：`/data/anr` `/data/tombstones`
- 新增清理配置：
```
info.muge.appshare   #AppShare
com.coolapk.market   #Coolapk
```

# UPDATE
- 清理顯示大小單位支援動態單位顯示、F2FS-GC 統計改為顯示大小而不是段數量
- App 快取清理針對多使用者效能最佳化，不再重複取得 App 列表
- 檔案歸類大小限制功能完整支援為每個附檔名配置限制
- Timed、StopCached 行程訊號處理最佳化
- 規則目錄綁定規則支援單引號包裹解析
- LOG 記錄機制更新
- App UI 排版更新
- 註解支援最佳化

# FIX
- Timed 熱更新增加移動事件監控，修復原子修改配置不更新問題
- 修復配置備份異常
