# VersionCode 82 To 83

完整架構請前往 [**CI構建**](https://github.com/FLYCOM-E/ClearBox/actions)

## 更新：

取消清理臨時關閉 SELinux

開放 Timed 自定義配置，解析優化

修復錯誤重定向導致更新配置無法看到可能的錯誤消息

完整多拓展儲存支持，包括 Micro Card、USB 儲存等

阻止更新不再支持外置儲存拓展，因此原因，您如果有插入拓展儲存並打開阻止安裝功能，此版本更新前請暫時關閉

### 本期感謝名單

Thanks GitHub @Amogus 提交 CI 自動構建配置

### **•  您知道嗎？**

您可以利用此模組 **Timed** 運行自定義任務！請按照以下配置：

配置目錄：**`/data/adb/wipe_cache/TimedConfig`**

在此目錄下創建一個自定義名稱 **.conf** 文件，填入並修改以下三行：

```conf
time=時間/單位（目前支持M/分、H/時、D/天）  
date=0  
run=要運行的程序及參數（注意暫不支持“"", ''”等參數形式，這會被錯誤解析）  
post=通知標題/通知內容（發送通知。這是一个可选參數，不建議在分鐘級任務加這個）
```

實例：

```conf
time=1/H  
date=0  
run=/system/bin/true  
post=Test Run/已運行“/system/bin/true”
```

以上配置會在每小時運行一次 **`/system/bin/true`** 命令，並發送一個通知

填寫完後保存，您需要手動運行 **立即生效當前配置** 或 **重啟** 後 **該任務才會生效！**

注：天級任務僅會在凌晨0~6點運行！
