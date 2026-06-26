# Version 5.1.2 (94)

# ADD
- 文件归类支持设置归类目录，支持填写绝对/相对路径
- 系统缓存清理新增: `/data/anr` `/data/tombstones`
- 新增清理配置：
```
info.muge.appshare   #AppShare
com.coolapk.market   #Coolapk
```

# UPDATE
- 清理显示大小单位支持动态单位显示、F2FS-GC 统计改为显示大小而不是段数量
- App 缓存清理针对多用户性能优化，不再重复获取 App 列表
- 文件归类大小限制功能完整支持为每个后缀配置限制
- Timed、StopCached 进程信号处理优化
- 规则目录绑定规则支持单引号包裹解析
- LOG 记录机制更新
- App UI 排版更新
- 注释支持优化

# FIX
- Timed 热更新增加移动事件监控，修复原子修改配置不更新问题
- 修复配置备份异常
