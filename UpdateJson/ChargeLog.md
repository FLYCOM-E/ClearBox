# Version 4.9.9 Beta
（Not is debug）

## Update：

模块启动初始化脚本增加分阶段（first and second）修复部分设备启动完成信号较早，data未挂载完成导致配置生成异常及可能导致内部储存掉挂载bug

为修复crond引起的自动功能bug，引入自定义Timed代替crond，为完全功能自动化打基础（资源占用完全可忽略

修复上版StopCached进程炸掉不运行的bug，系post通知函数bug主动降权（shell 2000）

修复CacheClean的一个潜在资源泄露问题，增大缓冲区大小，增加对获取包名的检查

增加定期清理自定义规则目录、定期清理软件缓存功能

优化软件缓存清理速度，减少重复调用开销

更多bug修复与改进