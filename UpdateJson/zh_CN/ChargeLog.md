# Version 5.1.5 (98)

# ADD
- NCDU 历史自动清理
- 适配日语
- 适配韩语

# UPDATE
- NCDU 历史功能改为 SQlite3 储存数据
- 优化 App 壁纸模式
- 优化多语言匹配，减少重复逻辑

# FIX
- 部分设备 settings get system system_locales 返回 null 导致语言回退，现在会优先回退至系统默认值
- 修复一些内存泄漏问题
