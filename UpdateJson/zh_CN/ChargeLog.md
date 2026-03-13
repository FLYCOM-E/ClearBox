# Version Code 87

## Add
- Timed 新增时间范围功能，在配置中新增 in 字段，按如下填写：

```
in=开始时间/结束时间（在X点到X点之间运行。这是一个可选参数，填写值请不要超过23，注意非天级任务可能会在此时段内多次运行）
```

实例：

```conf
time=1/D
date=0
run=/system/bin/sync
in=0/5
post=Test Run/已运行“/system/bin/sync”
```

以上配置会在每天凌晨 0~5 点之间运行一次 **`/system/bin/sync`** 命令，并发送一条通知

- 一键清理增加清理项：App 规则清理，一键清理所有 App
- 测试上线部分 App 清理规则，模块会自动处理存在关系：

```
Google Play  ： com.android.vending
Google Photo ： com.google.android.apps.photos
GMS          ： com.google.android.gms
Google Input ： com.google.android.inputmethod.latin
YouTube      ： com.google.android.youtube
Edge         ： com.microsoft.emmx
Outlook      ： com.microsoft.office.outlook
云湖          ： com.yhchat.app
拼多多商家版   ： com.xunmeng.merchant
```

## Update
- Timed 配置执行命令支持 Shell 语法，新增分钟对齐
- 修复 App 规则清理的部分 Bug
- 修复配置备份脚本删除日志名称错误 Bug
- 修复终端 TUI 下无法看到清理统计信息
- 优化 Shell 脚本
