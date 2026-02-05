# VersionCode 82 To 83

完整架构请前往 [**CI构建**](https://github.com/FLYCOM-E/ClearBox/actions)

## Update：

取消清理临时关闭 SELinux

开放 Timed 自定义配置，解析优化

修复错误重定向导致更新配置无法看到可能的错误消息

完整多拓展储存支持，包括 Micro Card、USB 储存等

阻止更新不再支持外置储存拓展，因此原因，您如果有插入拓展储存并打开阻止安装功能，此版本更新前请暂时关闭

### 本期感谢名单

Thanks GitHub @Amogus 提交 CI 自动构建配置

### **•  您知道吗？**

您可以利用此模块 **Timed** 运行自定义任务！请按照以下配置：

配置目录：**`/data/adb/wipe_cache/TimedConfig`**

在此目录下创建一个自定义名称 **.conf** 文件，填入并修改以下三行：

```conf
time=时间/单位（目前支持M/分、H/时、D/天）
date=0
run=要运行的程序及参数（注意暂不支持“"", ''”等参数形式，这会被错误解析）
post=通知标题/通知内容（发送通知。这是一个可选参数，不建议在分钟级任务加这个）
```

实例：

```conf
time=1/H
date=0
run=/system/bin/true
post=Test Run/已运行“/system/bin/true”
```

以上配置会在每小时运行一次 **`/system/bin/true`** 命令，并发送一个通知

填写完后保存，您需要手动运行 **立即生效当前配置** 或 **重启** 后 **该任务才会生效！**

注：天级任务仅会在凌晨0~6点运行！
