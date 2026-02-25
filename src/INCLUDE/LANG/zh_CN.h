// 通用宏
#define L_NOT_USE_ROOT " » 请使用 Root 权限！\n"
#define L_ARGS_FAILED " » 传入参数不足！\n"
#define L_ARGS_FAILED_2 " » 传入参数错误！\n"
#define L_CONFIG_PATH_NOTFOUND " » 配置路径不存在/不可访问！\n"
#define L_CONFIG_NOTFOUND " » %s 配置文件不存在！\n"
#define L_CONFIG_PATH_TOOLONG " » 配置路径过长！\n"
#define L_BIN_PATH_NOTFOUND " » Bin 路径不存在/不可访问！\n"
#define L_BIN_PATH_TOOLONG " » Bin 路径过长！\n"
#define L_PACKAGE_TOOLONG " » 传入包名过长！限制 %d\n"
#define L_ARG_PACKAGE_ERR " » 未传入包名！\n"
#define L_MODE_TOOLONG " » 模式名称过长！\n"
#define L_ARG_MODE_ERR " » 未传入模式！\n"
#define L_MODE_ERR " » 未知模式！[%s]\n"
#define L_ARG_CONFIGPATH_ERR " » 未传入配置目录！\n"
#define L_NOCONFIG " » 未找到任何配置！\n"
#define L_ARG_BINPATH_ERR " » 未传入 Bin 目录！\n"
#define L_OPEN_PATH_FAILED " » %s 目录打开失败！\n"
#define L_OPEN_FILE_FAILED " » %s 文件打开失败！\n"
#define L_PATH_NOTISDIR " » %s 不是一个目录！\n"
#define L_PATH_NOTISFILE " » %s 不是一个文件！\n"
#define L_GET_APPLIST_ERROR " » 获取软件列表失败！\n"
#define L_GET_SYSTEM_VERSION_ERR " » 获取系统版本失败！\n"
#define L_PATH_STAT_FAILED " » 获取 %s 信息失败！\n"
#define L_W_SETPROP_ERR " » 警告：PROP设置失败！\n"
#define L_MOVE_ERROR " » %s 移动失败\n"
#define L_DELETE_ERR " » 删除 %s 失败！\n"
#define L_SERVER_START_ERR " » 进程启动失败！\n"

// For StopCached
#define L_SCD_START_SUCCESS " » 缓存阻止已启动！\n"
#define L_SCD_GETAPP_ERR_EXIT " » 进程已退出，原因：GETTOPAPP ERROR\n"

// For Timed
#define L_TD_CONFIG_ERROR " » %s 配置错误，Skip.\n"
#define L_TD_LINE_ERR_VALUE " » %s 配置：%d 行 %s 对应值错误。已忽略该行\n"
#define L_TD_LINE_ERR_KEY " » %s 配置：%d 行 %s 未定义错误。已忽略该行\n"
#define L_TD_MAX_CONFIG " » 已达到加载配置数量限制 %d。已跳过更多配置\n"
#define L_TD_CONFIG_SUCCESS " » %s 配置加载成功\n"
#define L_TD_START_SUCCESS " » 服务启动成功！共加载 %d 个配置！\n"
#define L_TD_W_POST_TITLE_TOOLONG " » 警告：%s 配置通知标题长度超过限制！已截断\n"
#define L_TD_W_POST_MESSAGE_TOOLONG " » 警告：%s 配置通知内容长度超过限制！已截断\n"

// For AppClean.c
#define L_AC_CLEAR " » 清理 %s &\n"
#define L_AC_CONFIG_NOTFOUND " » 无App清理配置！\n"
#define L_AC_CONFIG_APP_NOTFOUND " » %s 配置指定软件未找到！\n"
#define L_AC_CONFIG_PACKAGE_ERR " » %s 配置声明错误，未正确填写包名！\n"
#define L_AC_CONFIG_APPNAME_ERR " » %s 配置声明错误，未正确填写软件名称！\n"
#define L_AC_CONFIG_ERR " » %s 配置错误！请在第一行正确填写声明\n"
#define L_AC_CONFIG_ERR_1 " » %s 配置第 %d 行存在危险错误：从“／”开始！\n"
#define L_AC_CONFIG_ERR_2 " » %s 配置第 %d 行存在错误：路径逃逸！\n"
#define L_AC_CLEAR_PATH_ERR " » 清理 %s 失败！\n"
#define L_AC_CLEAR_PATH_SUCCESS " » %s 处理成功！%ld 兆\n"
#define L_AC_CLEAN_SUCCESSFUL " » 软件规则处理成功！\n"
#define L_AC_CLEAN_FAILED " » 未找到指定软件配置或发生错误！\n"

// For CacheClean.c
#define L_CC_CLEAR_FAILED " » 内部储存软件缓存清理失败\n"
#define L_CC_CLEAR_SUCCESSFUL " » 内部储存软件缓存清理完成\n » 共清理：%d兆\n"
#define L_CC_CLEAR_FAILED_SD " » 外部储存软件缓存清理失败\n"
#define L_CC_CLEAR_SUCCESSFUL_SD " » 外部储存软件缓存清理完成\n » 共清理：%d兆\n"
#define L_CC_CLEAR " » %s 缓存已清除\n"
#define L_CC_CLEAR_SKIP " » 跳过 %s\n"
#define L_CC_CLEAR_APPCACHE_DONE " » 共清理 %d 款软件，%d 款软件无需清理\n"
#define L_CC_CLEAR_SYSTEMCACHE " » 系统缓存已清空！建议重启系统！\n"

// For Dexoat.c
#define L_DO_RUN_SYSTEM " » 正在进行系统默认 Dexoat...\n"
#define L_DO_RUN_CUST " » 正在运行模式 %s 编译...\n"

// For F2fs_GC.c
#define L_FG_ERR_NOF2FS " » 您的设备不是 F2FS 文件系统\n » 维护仅支持 F2FS 环境！\n"
#define L_FG_ERR_CHECK " » 您的设备不支持当前GC功能\n"
#define L_FG_W_GET_FREE " » 警告：获取当前空闲段失败！\n"
#define L_FG_W_GET_DIRTY " » 警告：获取当前脏段失败！\n"
#define L_FG_DIRTY " » 目前脏段: %d\n"
#define L_FG_FREE " » 目前空闲段: %d\n\n"
#define L_FG_ERR_OPENSYSFS " » GC启动失败! 节点打开失败！\n"
#define L_FG_START " » GC已开始, 请您耐心等待，建议挂后台！\n"
#define L_FG_ERR_WRITESYSFS " » GC启动失败! 节点写入失败！\n"
#define L_FG_ERR_TIMEOUT " » GC等待超时，已结束等待！\n"
#define L_FG_END " » GC运行完成，已结束运行！\n"
#define L_FG_RUN_MS " » 已运行 %d 分 %d 秒...\n"
#define L_FG_RUN_M " » 已运行 %d 分...\n"
#define L_FG_RUN_S " » 已运行 %d 秒...\n"
#define L_FG_END_DIRTY " » 磁盘脏块减少 %d\n"
#define L_FG_END_DIRTY_2 " » 磁盘脏块增加 %d\n » GC可能仍在优化或并不适合您的设备！\n"
#define L_FG_DONE " » GC已完成！\n"
#define L_FG_FAST_GC_DONE " » 快速磁盘优化完成，可以试试更激进的GC优化哦 (・∀・)\n"

// For FileManager.c
#define L_FM_ALL_FAILED_STORAGE " » 内部储存文件归类失败！\n"
#define L_FM_ALL_SUCCESSFUL_STORAGE " » 内部储存文件归类成功！\n"
#define L_FM_ALL_FAILED_SD " » 外部储存文件归类失败！\n"
#define L_FM_ALL_SUCCESSFUL_SD " » 外部储存文件归类成功！\n"

#define L_FM_CR_FAILED_STORAGE " » 内部储存文件清理失败！\n"
#define L_FM_CR_SUCCESSFUL_STORAGE " » 内部储存文件清理成功！\n"
#define L_FM_CR_FAILED_SD " » 外部储存文件清理失败！\n"
#define L_FM_CR_SUCCESSFUL_SD " » 外部储存文件清理成功！\n"

#define L_FM_ALL_START " » 正在归类 %s ...\n"
#define L_FM_ALL_END " » 已归类 %d 个 %s\n"
#define L_FM_CR_START " » 正在清理 %s ...\n"
#define L_FM_CR_END " » 已清理 %d 个 %s\n"

// For FreeZer.c
#define L_OPEN_FREEZER_SUCCESSFUL " » 已打开安卓原生墓碑 (^^)\n"

// For SetInstall.c
#define L_SI_OPEN_FAILED_STORAGE " » 内部储存阻止更新失败！\n"
#define L_SI_OPEN_SUCCESSFUL_STORAGE " » 内部储存已阻止更新！\n"
#define L_SI_OFF_FAILED_STORAGE " » 内部储存关闭阻止更新失败！\n"
#define L_SI_OFF_SUCCESSFUL_STORAGE " » 内部储存已关闭已阻止更新！\n"

// For SetStorage.c
#define L_SS_OPEN_STOP_STORAGE_SUCCESSFUL " » 已开启内部储存固定！\n"
#define L_SS_OPEN_STOP_STORAGE_FAILED " » 开启内部储存固定失败！\n"
#define L_SS_OFF_STOP_STORAGE_SUCCESSFUL " » 已关闭内部储存固定！\n"
#define L_SS_OFF_STOP_STORAGE_FAILED " » 关闭内部储存固定失败！\n"

// For StorageClean.c
#define L_SC_SUCCESSFUL_STORAGE " » 内部储存垃圾清理完成！\n"
#define L_SC_SUCCESSFUL_SD " » 外部储存 %s 垃圾清理完成！\n"
#define L_SC_CLEAR " » 清理 %s 储存缓存\n"
#define L_SC_CLEAR_ERR " » 清理 %s 储存缓存失败！\n"
#define L_SC_CLEAR_CACHE_DONE " » 共清理 %d 个软件缓存！\n"
#define L_SC_CLEAR_DIRTY " » 清理成功！共清理 %d 个空文件/文件夹及Log文件\n"
#define L_SC_CLEAR_DIRTY_ERR " » 空文件/夹及Log文件清理失败！\n"

// For StorageRuleClean.c
#define L_SR_OPEN_CONFIG_ERR " » %s 配置打开失败！自动跳过\n"
#define L_SR_START " » 处理 %s 配置📍\n"
#define L_SR_W_CONFIG_STATTPATH_ERR " » 警告：%s 配置指定初始路径不存在！\n"
#define L_SR_CLEAR_SUCCESSFUL " » 清理 %s 成功！%ld 兆\n"
#define L_SR_CLEAR_FAILED " » 清理 %s 失败\n"
#define L_SR_LINE_FAILED_PATH_ERR " » %d 行错误：路径错误/无法访问\n"
#define L_SR_END " » 自定义目录处理完成！\n"
