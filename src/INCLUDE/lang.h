// By ClearBox

typedef enum
{
    LANG_zh_CN = 0,
    LANG_zh_TW = 1,
    LANG_ru_RU = 2,
    LANG_en_US = 3,
    LANG_ko_KR = 4,
} LangType;

typedef struct
{
    const char * l_lang_name;
    
    const char * l_not_use_root;
    const char * l_args_failed;
    const char * l_mode_err;
    const char * l_noconfig;
    const char * l_open_path_failed;
    const char * l_open_file_failed;
    const char * l_mkdir_error;
    const char * l_mount_error;
    const char * l_get_applist_error;
    const char * l_get_sys_version_err;
    const char * l_w_setprop_err;
    const char * l_move_error;
    const char * l_delete_error;
    const char * l_server_start_err;
    const char * l_path_file_error;
    
    const char * l_clear_all;
    const char * l_clear_cache;
    const char * l_clear_sys_cache;
    const char * l_cust_storage_clean;
    const char * l_storage_clean;
    const char * l_file_clean;
    const char * l_cust_app_clean;
    const char * l_app_clean;
    const char * l_file_all;
    const char * l_stop_install;
    const char * l_lock_storage;
    const char * l_fast_disk_gc;
    const char * l_disk_gc;
    const char * l_sys_dexoat;
    const char * l_cust_dexoat;
    const char * l_reset_dexoat;
    const char * l_config_mg;
    const char * l_stop_cache_daemon;
    const char * l_time_daemon;
    const char * l_path_bind;
    const char * l_storage_scan;
    
    const char * l_scd_start_success;
    const char * l_scd_getapp_err_exit;
    const char * l_td_config_error;
    const char * l_td_line_err_value;
    const char * l_td_line_err_key;
    const char * l_td_max_config;
    const char * l_td_config_success;
    const char * l_td_start_success;
    const char * l_td_w_post_title_toolong;
    const char * l_td_w_post_message_toolong;
    const char * l_td_config_write_error;
    const char * l_td_watch_config_dir_error;
    const char * l_td_load_config_success;
    const char * l_ac_clear;
    const char * l_ac_config_app_notfound;
    const char * l_ac_config_package_err;
    const char * l_ac_config_appname_err;
    const char * l_ac_config_err;
    const char * l_ac_config_err_1;
    const char * l_ac_config_err_2;
    const char * l_ac_clear_path_err;
    const char * l_ac_clear_path_success;
    const char * l_ac_clean_successful;
    const char * l_ac_clean_failed;
    const char * l_cc_clear_failed;
    const char * l_cc_clear_successful;
    const char * l_cc_clear_failed_sd;
    const char * l_cc_clear_successful_sd;
    const char * l_cc_clear;
    const char * l_cc_clear_skip;
    const char * l_cc_clear_appcache_done;
    const char * l_cc_clear_systemcache;
    const char * l_do_run_system;
    const char * l_do_run_cust;
    const char * l_fg_err_nof2fs;
    const char * l_fg_err_check;
    const char * l_fg_dirty;
    const char * l_fg_free;
    const char * l_fg_start;
    const char * l_fg_err_writesysfs;
    const char * l_fg_err_timeout;
    const char * l_fg_end;
    const char * l_fg_run_ms;
    const char * l_fg_run_s;
    const char * l_fg_end_dirty;
    const char * l_fg_end_dirty_2;
    const char * l_fg_fast_gc_done;
    const char * l_fg_dirty_low;
    const char * l_fm_all_failed_storage;
    const char * l_fm_all_successful_storage;
    const char * l_fm_all_failed_sd;
    const char * l_fm_all_successful_sd;
    const char * l_fm_cr_failed_storage;
    const char * l_fm_cr_successful_storage;
    const char * l_fm_cr_failed_sd;
    const char * l_fm_cr_successful_sd;
    const char * l_fm_all_start;
    const char * l_fm_all_end;
    const char * l_fm_cr_start;
    const char * l_fm_cr_end;
    const char * l_fm_min_size_error;
    const char * l_fm_size_max_error;
    const char * l_fm_size_min_error;
    const char * l_fm_size_error;
    const char * l_open_freezer_successful;
    const char * l_si_open_failed_storage;
    const char * l_si_open_successful_storage;
    const char * l_si_off_failed_storage;
    const char * l_si_off_successful_storage;
    const char * l_ss_open_stop_storage_successful;
    const char * l_ss_open_stop_storage_failed;
    const char * l_ss_off_stop_storage_successful;
    const char * l_ss_off_stop_storage_failed;
    const char * l_sc_successful_storage;
    const char * l_sc_successful_sd;
    const char * l_sc_clear;
    const char * l_sc_clear_err;
    const char * l_sc_clear_cache_done;
    const char * l_sc_clear_dirty_err;
    const char * l_sr_start;
    const char * l_sr_w_config_stattpath_err;
    const char * l_sr_clear_successful;
    const char * l_sr_clear_failed;
    const char * l_sr_line_failed_path_err;
    const char * l_sr_end;
    const char * l_bm_mount_success;
    const char * l_bm_error_is_sdcardfs;
} LangPack;

static const LangPack lang_packs[] =
{
    [LANG_zh_CN] =
    {
        .l_lang_name = "zh_CN",
        
        .l_not_use_root = " » 请使用 Root 权限！#%u\n",
        .l_args_failed = " » 传入参数不足！\n",
        .l_mode_err = " » 未知模式！[%s]\n",
        .l_noconfig = " » 未找到任何配置！\n",
        .l_open_path_failed = " » %s 目录打开失败: %s\n",
        .l_open_file_failed = " » %s 文件打开失败: %s\n",
        .l_mkdir_error = " » %s 目录创建失败: %s\n",
        .l_mount_error = " » %s 挂载至 %s 失败: %s\n",
        .l_get_applist_error = " » 获取软件列表失败！\n",
        .l_get_sys_version_err = " » 获取系统版本失败！\n",
        .l_w_setprop_err = " » 警告：PROP设置失败！\n",
        .l_move_error = " » %s 移动失败: %s\n",
        .l_delete_error = " » 删除 %s 失败: %s\n",
        .l_server_start_err = " » 进程启动失败: %s\n",
        .l_path_file_error = " » PATH 配置错误，自动重写 ... 请您重新尝试，如失败请删除 %s 后更新配置再试\n",
        
        .l_clear_all = "一键优化清理\n",
        .l_clear_cache = "清理软件缓存\n",
        .l_clear_sys_cache = "清理系统缓存\n",
        .l_cust_storage_clean = "自定义规则储存清理\n",
        .l_storage_clean = "储存目录清理\n",
        .l_file_clean = "文件清理\n",
        .l_cust_app_clean = "自定义软件规则清理\n",
        .l_app_clean = "软件规则清理\n",
        .l_file_all = "文件归类\n",
        .l_stop_install = "阻止安装\n",
        .l_lock_storage = "储存锁定\n",
        .l_fast_disk_gc = "快速磁盘优化\n",
        .l_disk_gc = "磁盘 GC\n",
        .l_sys_dexoat = "系统 DEXOAT\n",
        .l_cust_dexoat = "自定义 DEXOAT\n",
        .l_reset_dexoat = "恢复 DEXOAT\n",
        .l_config_mg = "配置备份/恢复\n",
        .l_stop_cache_daemon = "阻止缓存 (Daemon)\n",
        .l_time_daemon = "自动任务 (Daemon)\n",
        .l_path_bind = "规则目录绑定（重定向）\n",
        .l_storage_scan = "储存分析（NCDU）\n",
        
        .l_scd_start_success = " » 缓存阻止已启动！#%u",
        .l_scd_getapp_err_exit = " » 进程已退出，原因：GETTOPAPP ERROR\n",
        .l_td_config_error = " » %s 配置错误，Skip.\n",
        .l_td_line_err_value = " » %s 配置：%d 行 %s 对应值错误。已忽略该行\n",
        .l_td_line_err_key = " » %s 配置：%d 行 %s 未定义错误。已忽略该行\n",
        .l_td_max_config = " » 已达到加载配置数量限制 %d。已跳过更多配置\n",
        .l_td_config_success = " » %s 配置加载成功\n",
        .l_td_start_success = " » 服务启动成功！共加载 %d 个配置！#%u",
        .l_td_w_post_title_toolong = " » 警告：%s 配置通知标题长度超过限制！已截断\n",
        .l_td_w_post_message_toolong = " » 警告：%s 配置通知内容长度超过限制！已截断\n",
        .l_td_config_write_error = " » %s 配置回写失败.\n",
        .l_td_watch_config_dir_error = " » 热更新配置初始化失败: %s\n",
        .l_td_load_config_success = " » %s 配置已更新\n",
        .l_ac_clear = " » 清理 %s &\n",
        .l_ac_config_app_notfound = " » %s 配置指定软件未找到！\n",
        .l_ac_config_package_err = " » %s 配置声明错误，未正确填写包名！\n",
        .l_ac_config_appname_err = " » %s 配置声明错误，未正确填写软件名称！\n",
        .l_ac_config_err = " » %s 配置错误！请在第一行正确填写声明\n",
        .l_ac_config_err_1 = " » %s 配置第 %d 行存在危险错误：从“/”开始！\n",
        .l_ac_config_err_2 = " » %s 配置第 %d 行存在错误：路径逃逸！\n",
        .l_ac_clear_path_err = " » 清理 %s 失败！\n",
        .l_ac_clear_path_success = " » %s 处理成功！%.2f %c\n",
        .l_ac_clean_successful = " » 软件规则处理成功！共清理：%.2f %c\n",
        .l_ac_clean_failed = " » 未找到指定软件配置或发生错误！#%d\n",
        .l_cc_clear_failed = " » 内部储存软件缓存清理失败\n",
        .l_cc_clear_successful = " » 内部储存软件缓存清理完成\n » 共清理：%.2f %c\n",
        .l_cc_clear_failed_sd = " » 外部储存软件缓存清理失败\n",
        .l_cc_clear_successful_sd = " » 外部储存软件缓存清理完成\n » 共清理：%.2f %c\n",
        .l_cc_clear = " » %s 缓存已清除\n",
        .l_cc_clear_skip = " » 跳过 %s\n",
        .l_cc_clear_appcache_done = " » 共清理 %d 款软件，%d 款软件无需清理\n",
        .l_cc_clear_systemcache = " » 系统缓存已清空！建议重启系统！\n",
        .l_do_run_system = " » 正在进行系统默认 Dexoat...\n",
        .l_do_run_cust = " » 正在运行模式 %s 编译...\n",
        .l_fg_err_nof2fs = " » 您的设备不是 F2FS 文件系统\n » 维护仅支持 F2FS 环境！\n",
        .l_fg_err_check = " » 您的设备不支持当前GC功能\n",
        .l_fg_dirty = " » 目前脏段: %ld\n",
        .l_fg_free = " » 目前空闲段: %ld\n\n",
        .l_fg_start = " » 后台 GC 已开始, 请您耐心等待\n",
        .l_fg_err_writesysfs = " » GC启动失败! 节点写入失败！\n",
        .l_fg_err_timeout = " » GC等待超时，已结束等待！\n",
        .l_fg_end = " » GC运行完成，已结束运行！\n",
        .l_fg_run_ms = " » 已运行 %d 分 %d 秒...\n",
        .l_fg_run_s = " » 已运行 %d 秒...\n",
        .l_fg_end_dirty = " » 磁盘脏块减少 %ld, 共 %.2f %c\n",
        .l_fg_end_dirty_2 = " » 磁盘脏块增加 %ld\n » GC可能仍在优化或并不适合您的设备！\n",
        .l_fg_fast_gc_done = " » 快速磁盘优化完成，可以试试更激进的GC优化哦 (・∀・)\n",
        .l_fg_dirty_low = " » 您的磁盘状态极佳，无需优化 (・∀・)\n",
        .l_fm_all_failed_storage = " » 内部储存文件归类失败！\n",
        .l_fm_all_successful_storage = " » 内部储存文件归类成功！\n",
        .l_fm_all_failed_sd = " » 外部储存文件归类失败！\n",
        .l_fm_all_successful_sd = " » 外部储存文件归类成功！\n",
        .l_fm_cr_failed_storage = " » 内部储存文件清理失败！\n",
        .l_fm_cr_successful_storage = " » 内部储存文件清理成功！\n",
        .l_fm_cr_failed_sd = " » 外部储存文件清理失败！\n",
        .l_fm_cr_successful_sd = " » 外部储存文件清理成功！\n",
        .l_fm_all_start = " » 正在归类 %s ...\n",
        .l_fm_all_end = " » 已归类 %d 个 %s\n",
        .l_fm_cr_start = " » 正在清理 %s ...\n",
        .l_fm_cr_end = " » 已清理 %d 个 %s\n",
        .l_fm_min_size_error = " » 警告：%s 配置 MIN 大小大于 MAX，MIN 已忽略！\n",
        .l_fm_size_max_error = " » 警告：%s 配置 MAX 声明填写错误！已忽略\n",
        .l_fm_size_min_error = " » 警告：%s 配置 MIN 声明填写错误！已忽略\n",
        .l_fm_size_error = " » 警告：%s 配置大小范围单位填写错误！已忽略对应值\n",
        .l_open_freezer_successful = " » 已打开安卓原生墓碑 (^^)\n",
        .l_si_open_failed_storage = " » 内部储存阻止更新失败: %s\n",
        .l_si_open_successful_storage = " » 内部储存已阻止更新！\n",
        .l_si_off_failed_storage = " » 内部储存关闭阻止更新失败: %s\n",
        .l_si_off_successful_storage = " » 内部储存已关闭已阻止更新！\n",
        .l_ss_open_stop_storage_successful = " » 已开启内部储存固定！\n",
        .l_ss_open_stop_storage_failed = " » 开启内部储存固定失败: %s\n",
        .l_ss_off_stop_storage_successful = " » 已关闭内部储存固定！\n",
        .l_ss_off_stop_storage_failed = " » 关闭内部储存固定失败: %s\n",
        .l_sc_successful_storage = " » 内部储存垃圾清理完成！\n » 共清理 %d 个空文件/文件夹及Log文件\n",
        .l_sc_successful_sd = " » 外部储存 %s 垃圾清理完成！\n » 共清理 %d 个空文件/文件夹及Log文件\n",
        .l_sc_clear = " » 清理 %s 储存缓存\n",
        .l_sc_clear_err = " » 清理 %s 储存缓存失败！\n",
        .l_sc_clear_cache_done = " » 共清理 %d 个软件缓存！\n",
        .l_sc_clear_dirty_err = " » 空文件/夹及Log文件清理失败！\n",
        .l_sr_start = " » 处理 %s 配置📍\n",
        .l_sr_w_config_stattpath_err = " » 警告：%s 配置指定初始路径不存在！\n",
        .l_sr_clear_successful = " » 清理 %s 成功！%.2f %c\n",
        .l_sr_clear_failed = " » 清理 %s 失败\n",
        .l_sr_line_failed_path_err = " » %d 行错误：路径错误/无法访问\n",
        .l_sr_end = " » 自定义目录处理完成！\n",
        .l_bm_mount_success = " » 已成功绑定目录 %s 至 %s\n",
        .l_bm_error_is_sdcardfs = " » 您的设备使用 sdcardfs 方案，此功能无法正常使用！\n",
    },
    [LANG_zh_TW] =
    {
        .l_lang_name = "zh_TW",
        
        .l_not_use_root = " » 請使用 Root 權限！#%u\n",
        .l_args_failed = " » 傳入參數不足！\n",
        .l_mode_err = " » 未知模式！[%s]\n",
        .l_noconfig = " » 未找到任何配置！\n",
        .l_open_path_failed = " » %s 目錄開啟失敗: %s\n",
        .l_open_file_failed = " » %s 檔案開啟失敗: %s\n",
        .l_mkdir_error = " » %s 目錄建立失敗: %s\n",
        .l_mount_error = " » %s 掛載至 %s 失敗: %s\n",
        .l_get_applist_error = " » 獲取軟體列表失敗！\n",
        .l_get_sys_version_err = " » 獲取系統版本失敗！\n",
        .l_w_setprop_err = " » 警告：PROP設定失敗！\n",
        .l_move_error = " » %s 移動失敗: %s\n",
        .l_delete_error = " » 刪除 %s 失敗: %s\n",
        .l_server_start_err = " » 進程啟動失敗: %s\n",
        .l_path_file_error = " » PATH 配置錯誤，自動重寫 ... 請您重新嘗試，如失敗請刪除 %s 後更新配置再試\n",
        
        .l_clear_all = "一鍵優化清理\n",
        .l_clear_cache = "清理軟體快取\n",
        .l_clear_sys_cache = "清理系統快取\n",
        .l_cust_storage_clean = "自訂規則儲存清理\n",
        .l_storage_clean = "儲存目錄清理\n",
        .l_file_clean = "檔案清理\n",
        .l_cust_app_clean = "自訂軟體規則清理\n",
        .l_app_clean = "軟體規則清理\n",
        .l_file_all = "檔案歸類\n",
        .l_stop_install = "阻止安裝\n",
        .l_lock_storage = "儲存鎖定\n",
        .l_fast_disk_gc = "快速磁碟最佳化\n",
        .l_disk_gc = "磁碟 GC\n",
        .l_sys_dexoat = "系統 DEXOAT\n",
        .l_cust_dexoat = "自訂 DEXOAT\n",
        .l_reset_dexoat = "恢復 DEXOAT\n",
        .l_config_mg = "配置備份/恢復\n",
        .l_stop_cache_daemon = "阻止快取 (Daemon)\n",
        .l_time_daemon = "自動任務 (Daemon)\n",
        .l_path_bind = "規則目錄綁定（重新導向）\n",
        .l_storage_scan = "儲存分析（NCDU）\n",
        
        .l_scd_start_success = " » 快取阻止已啟動！#%u",
        .l_scd_getapp_err_exit = " » 進程已退出，原因：GETTOPAPP ERROR\n",
        .l_td_config_error = " » %s 配置錯誤，Skip.\n",
        .l_td_line_err_value = " » %s 配置：%d 行 %s 對應值錯誤。已忽略該行\n",
        .l_td_line_err_key = " » %s 配置：%d 行 %s 未定義錯誤。已忽略該行\n",
        .l_td_max_config = " » 已達到載入配置數量限制 %d。已跳過更多配置\n",
        .l_td_config_success = " » %s 配置載入成功\n",
        .l_td_start_success = " » 服務啟動成功！共載入 %d 個配置！#%u",
        .l_td_w_post_title_toolong = " » 警告：%s 配置通知標題長度超過限制！已截斷\n",
        .l_td_w_post_message_toolong = " » 警告：%s 配置通知內容長度超過限制！已截斷\n",
        .l_td_config_write_error = " » %s 配置回寫失敗.\n",
        .l_td_watch_config_dir_error = " » 熱更新配置初始化失敗: %s\n",
        .l_td_load_config_success = " » %s 配置已更新\n",
        .l_ac_clear = " » 清理 %s &\n",
        .l_ac_config_app_notfound = " » %s 配置指定軟體未找到！\n",
        .l_ac_config_package_err = " » %s 配置宣告錯誤，未正確填寫套件名稱！\n",
        .l_ac_config_appname_err = " » %s 配置宣告錯誤，未正確填寫軟體名稱！\n",
        .l_ac_config_err = " » %s 配置錯誤！請在第一行正確填寫宣告\n",
        .l_ac_config_err_1 = " » %s 配置第 %d 行存在危險錯誤：從 “/” 開始！\n",
        .l_ac_config_err_2 = " » %s 配置第 %d 行存在錯誤：路徑逃逸！\n",
        .l_ac_clear_path_err = " » 清理 %s 失敗！\n",
        .l_ac_clear_path_success = " » %s 處理成功！%.2f %c\n",
        .l_ac_clean_successful = " » 軟體規則處理成功！共清理：%.2f %c\n",
        .l_ac_clean_failed = " » 未找到指定軟體配置或發生錯誤！#%d\n",
        .l_cc_clear_failed = " » 內部儲存軟體快取清理失敗\n",
        .l_cc_clear_successful = " » 內部儲存軟體快取清理完成\n » 共清理：%.2f %c\n",
        .l_cc_clear_failed_sd = " » 外部儲存軟體快取清理失敗\n",
        .l_cc_clear_successful_sd = " » 外部儲存軟體快取清理完成\n » 共清理：%.2f %c\n",
        .l_cc_clear = " » %s 快取已清除\n",
        .l_cc_clear_skip = " » 跳過 %s\n",
        .l_cc_clear_appcache_done = " » 共清理 %d 款軟體，%d 款軟體無需清理\n",
        .l_cc_clear_systemcache = " » 系統快取已清空！建議重啟系統！\n",
        .l_do_run_system = " » 正在進行系統預設 Dexoat...\n",
        .l_do_run_cust = " » 正在執行模式 %s 編譯...\n",
        .l_fg_err_nof2fs = " » 您的裝置不是 F2FS 檔案系統\n » 維護僅支援 F2FS 環境！\n",
        .l_fg_err_check = " » 您的裝置不支援當前GC功能\n",
        .l_fg_dirty = " » 目前髒段: %ld\n",
        .l_fg_free = " » 目前空閒段: %ld\n\n",
        .l_fg_start = " » 後台 GC 已開始，請您耐心等待\n",
        .l_fg_err_writesysfs = " » GC啟動失敗! 節點寫入失敗！\n",
        .l_fg_err_timeout = " » GC等待逾時，已結束等待！\n",
        .l_fg_end = " » GC執行完成，已結束執行！\n",
        .l_fg_run_ms = " » 已執行 %d 分 %d 秒...\n",
        .l_fg_run_s = " » 已執行 %d 秒...\n",
        .l_fg_end_dirty = " » 磁碟髒塊減少 %ld，共 %.2f %c\n",
        .l_fg_end_dirty_2 = " » 磁碟髒塊增加 %ld\n » GC可能仍在最佳化或並不適合您的裝置！\n",
        .l_fg_fast_gc_done = " » 快速磁碟最佳化完成，可以試試更激進的GC最佳化哦 (・∀・)\n",
        .l_fg_dirty_low = " » 您的磁碟狀態極佳，無需最佳化 (・∀・)\n",
        .l_fm_all_failed_storage = " » 內部儲存檔案歸類失敗！\n",
        .l_fm_all_successful_storage = " » 內部儲存檔案歸類成功！\n",
        .l_fm_all_failed_sd = " » 外部儲存檔案歸類失敗！\n",
        .l_fm_all_successful_sd = " » 外部儲存檔案歸類成功！\n",
        .l_fm_cr_failed_storage = " » 內部儲存檔案清理失敗！\n",
        .l_fm_cr_successful_storage = " » 內部儲存檔案清理成功！\n",
        .l_fm_cr_failed_sd = " » 外部儲存檔案清理失敗！\n",
        .l_fm_cr_successful_sd = " » 外部儲存檔案清理成功！\n",
        .l_fm_all_start = " » 正在歸類 %s ...\n",
        .l_fm_all_end = " » 已歸類 %d 個 %s\n",
        .l_fm_cr_start = " » 正在清理 %s ...\n",
        .l_fm_cr_end = " » 已清理 %d 個 %s\n",
        .l_fm_min_size_error = " » 警告：%s 配置 MIN 大小大於 MAX，MIN 已自動失效！\n",
        .l_fm_size_max_error = " » 警告：%s 配置 MAX 宣告填寫錯誤！已忽略\n",
        .l_fm_size_min_error = " » 警告：%s 配置 MIN 宣告填寫錯誤！已忽略\n",
        .l_fm_size_error = " » 警告：%s 配置大小範圍單位填寫錯誤！已忽略對應值\n",
        .l_open_freezer_successful = " » 已開啟安卓原生墓碑 (^^)\n",
        .l_si_open_failed_storage = " » 內部儲存阻止更新失敗: %s\n",
        .l_si_open_successful_storage = " » 內部儲存已阻止更新！\n",
        .l_si_off_failed_storage = " » 內部儲存關閉阻止更新失敗: %s\n",
        .l_si_off_successful_storage = " » 內部儲存已關閉已阻止更新！\n",
        .l_ss_open_stop_storage_successful = " » 已開啟內部儲存固定！\n",
        .l_ss_open_stop_storage_failed = " » 開啟內部儲存固定失敗: %s\n",
        .l_ss_off_stop_storage_successful = " » 已關閉內部儲存固定！\n",
        .l_ss_off_stop_storage_failed = " » 關閉內部儲存固定失敗: %s\n",
        .l_sc_successful_storage = " » 內部儲存空間清理完成！\n » 共清理 %d 個空檔案/資料夾及 Log 檔案\n",
        .l_sc_successful_sd = " » 外部儲存裝置 %s 清理完成！\n » 共清理 %d 個空檔案/資料夾及 Log 檔案\n",
        .l_sc_clear = " » 正在清理 %s 儲存快取\n",
        .l_sc_clear_err = " » 清理 %s 儲存快取失敗！\n",
        .l_sc_clear_cache_done = " » 共清理 %d 個應用程式快取！\n",
        .l_sc_clear_dirty_err = " » 空檔案/資料夾及 Log 檔案清理失敗！\n",
        .l_sr_start = " » 處理 %s 配置📍\n",
        .l_sr_w_config_stattpath_err = " » 警告：%s 配置指定初始路徑不存在！\n",
        .l_sr_clear_successful = " » 清理 %s 成功！%.2f %c\n",
        .l_sr_clear_failed = " » 清理 %s 失敗\n",
        .l_sr_line_failed_path_err = " » %d 行錯誤：路徑錯誤/無法存取\n",
        .l_sr_end = " » 自訂目錄處理完成！\n",
        .l_bm_mount_success = " » 已成功綁定目錄 %s 至 %s\n",
        .l_bm_error_is_sdcardfs = " » 您的裝置使用 sdcardfs 方案，此功能無法正常使用！\n",
    },
    [LANG_ru_RU] =
    {
        .l_lang_name = "ru_RU",
        
        .l_not_use_root = " » Используйте права Root！#%u\n",
        .l_args_failed = " » Недостаточно аргументов！\n",
        .l_mode_err = " » Неизвестный режим！[%s]\n",
        .l_noconfig = " » Конфигурация не найдена！\n",
        .l_open_path_failed = " » Не удалось открыть каталог %s: %s\n",
        .l_open_file_failed = " » Не удалось открыть файл %s: %s\n",
        .l_mkdir_error = " » Не удалось создать каталог %s: %s\n",
        .l_mount_error = " » Не удалось смонтировать %s в %s: %s\n",
        .l_get_applist_error = " » Не удалось получить список приложений！\n",
        .l_get_sys_version_err = " » Не удалось получить версию системы！\n",
        .l_w_setprop_err = " » Предупреждение: Не удалось установить PROP！\n",
        .l_move_error = " » Не удалось переместить %s: %s\n",
        .l_delete_error = " » Не удалось удалить %s: %s\n",
        .l_server_start_err = " » Не удалось запустить процесс: %s\n",
        .l_path_file_error = " » Ошибка конфигурации PATH, автоматическая перезапись... Попробуйте снова, если не удастся, удалите %s и обновите конфигурацию\n",
    
        .l_clear_all = "Оптимизация и очистка в один клик\n",
        .l_clear_cache = "Очистка кэша приложений\n",
        .l_clear_sys_cache = "Очистка системного кэша\n",
        .l_cust_storage_clean = "Очистка хранилища по пользовательским правилам\n",
        .l_storage_clean = "Очистка каталогов хранилища\n",
        .l_file_clean = "Очистка файлов\n",
        .l_cust_app_clean = "Очистка приложений по пользовательским правилам\n",
        .l_app_clean = "Очистка по правилам для приложений\n",
        .l_file_all = "Классификация файлов\n",
        .l_stop_install = "Блокировка установки\n",
        .l_lock_storage = "Блокировка хранилища\n",
        .l_fast_disk_gc = "Быстрая оптимизация диска\n",
        .l_disk_gc = "Дисковая GC\n",
        .l_sys_dexoat = "Системный DEXOAT\n",
        .l_cust_dexoat = "Пользовательский DEXOAT\n",
        .l_reset_dexoat = "Сброс DEXOAT\n",
        .l_config_mg = "Резервное копирование/восстановление конфигурации\n",
        .l_stop_cache_daemon = "Блокировка кэша (демон)\n",
        .l_time_daemon = "Автоматические задачи (демон)\n",
        .l_path_bind = "Привязка каталога правил (перенаправление)\n",
        .l_storage_scan = "Анализ хранилища (NCDU)\n",
    
        .l_scd_start_success = " » Блокировка кэша запущена！#%u",
        .l_scd_getapp_err_exit = " » Процесс завершен, причина: GETTOPAPP ERROR\n",
        .l_td_config_error = " » Ошибка конфигурации %s, пропуск.\n",
        .l_td_line_err_value = " » %s конфигурация: строка %d, %s неверное значение. Строка проигнорирована\n",
        .l_td_line_err_key = " » %s конфигурация: строка %d, %s неопределенная ошибка. Строка проигнорирована\n",
        .l_td_max_config = " » Достигнут лимит загруженных конфигураций %d. Остальные пропущены\n",
        .l_td_config_success = " » Конфигурация %s загружена успешно\n",
        .l_td_start_success = " » Служба запущена успешно！Загружено %d конфигураций！#%u",
        .l_td_w_post_title_toolong = " » Предупреждение: заголовок уведомления конфигурации %s превышает лимит！Обрезано\n",
        .l_td_w_post_message_toolong = " » Предупреждение: содержимое уведомления конфигурации %s превышает лимит！Обрезано\n",
        .l_td_config_write_error = " » Не удалось записать конфигурацию %s.\n",
        .l_td_watch_config_dir_error = " » Не удалось инициализировать горячее обновление конфигурации: %s\n",
        .l_td_load_config_success = " » Конфигурация %s обновлена\n",
        .l_ac_clear = " » Очистка %s &\n",
        .l_ac_config_app_notfound = " » Приложение, указанное в конфигурации %s, не найдено！\n",
        .l_ac_config_package_err = " » Ошибка объявления в конфигурации %s: неверно указан пакет！\n",
        .l_ac_config_appname_err = " » Ошибка объявления в конфигурации %s: неверно указано имя приложения！\n",
        .l_ac_config_err = " » Ошибка конфигурации %s! Укажите объявление в первой строке\n",
        .l_ac_config_err_1 = " » Опасная ошибка в строке %d конфигурации %s: начинается с \"/\"！\n",
        .l_ac_config_err_2 = " » Ошибка в строке %d конфигурации %s: выход за пределы пути！\n",
        .l_ac_clear_path_err = " » Не удалось очистить %s！\n",
        .l_ac_clear_path_success = " » %s обработано успешно！%.2f %c\n",
        .l_ac_clean_successful = " » Правила для приложений обработаны успешно！Очищено: %.2f %c\n",
        .l_ac_clean_failed = " » Конфигурация для указанного приложения не найдена или ошибка！#%d\n",
        .l_cc_clear_failed = " » Не удалось очистить кэш приложений во внутреннем хранилище\n",
        .l_cc_clear_successful = " » Очистка кэша приложений во внутреннем хранилище завершена\n » Очищено: %.2f %c\n",
        .l_cc_clear_failed_sd = " » Не удалось очистить кэш приложений во внешнем хранилище\n",
        .l_cc_clear_successful_sd = " » Очистка кэша приложений во внешнем хранилище завершена\n » Очищено: %.2f %c\n",
        .l_cc_clear = " » Кэш %s очищен\n",
        .l_cc_clear_skip = " » Пропуск %s\n",
        .l_cc_clear_appcache_done = " » Очищено %d приложений, %d приложений не требовали очистки\n",
        .l_cc_clear_systemcache = " » Системный кэш очищен！Рекомендуется перезагрузить систему！\n",
        .l_do_run_system = " » Выполняется системный Dexoat по умолчанию...\n",
        .l_do_run_cust = " » Выполняется компиляция в режиме %s...\n",
        .l_fg_err_nof2fs = " » Ваше устройство не использует файловую систему F2FS\n » Обслуживание поддерживается только в среде F2FS！\n",
        .l_fg_err_check = " » Ваше устройство не поддерживает текущую функцию GC\n",
        .l_fg_dirty = " » Текущие грязные сегменты: %ld\n",
        .l_fg_free = " » Текущие свободные сегменты: %ld\n\n",
        .l_fg_start = " » Фоновая GC запущена, пожалуйста, подождите\n",
        .l_fg_err_writesysfs = " » Не удалось запустить GC! Ошибка записи в узел！\n",
        .l_fg_err_timeout = " » Тайм-аут ожидания GC, ожидание завершено！\n",
        .l_fg_end = " » GC завершена, выполнение закончено！\n",
        .l_fg_run_ms = " » Выполнено %d мин %d сек...\n",
        .l_fg_run_s = " » Выполнено %d сек...\n",
        .l_fg_end_dirty = " » Грязные блоки диска уменьшены на %ld, всего %.2f %c\n",
        .l_fg_end_dirty_2 = " » Грязные блоки диска увеличились на %ld\n » GC может всё ещё оптимизировать или не подходит для вашего устройства！\n",
        .l_fg_fast_gc_done = " » Быстрая оптимизация диска завершена, можете попробовать более агрессивную GC (・∀・)\n",
        .l_fg_dirty_low = " » Состояние вашего диска отличное, оптимизация не требуется (・∀・)\n",
        .l_fm_all_failed_storage = " » Не удалось классифицировать файлы во внутреннем хранилище！\n",
        .l_fm_all_successful_storage = " » Классификация файлов во внутреннем хранилище выполнена успешно！\n",
        .l_fm_all_failed_sd = " » Не удалось классифицировать файлы во внешнем хранилище！\n",
        .l_fm_all_successful_sd = " » Классификация файлов во внешнем хранилище выполнена успешно！\n",
        .l_fm_cr_failed_storage = " » Не удалось очистить файлы во внутреннем хранилище！\n",
        .l_fm_cr_successful_storage = " » Очистка файлов во внутреннем хранилище выполнена успешно！\n",
        .l_fm_cr_failed_sd = " » Не удалось очистить файлы во внешнем хранилище！\n",
        .l_fm_cr_successful_sd = " » Очистка файлов во внешнем хранилище выполнена успешно！\n",
        .l_fm_all_start = " » Классификация %s ...\n",
        .l_fm_all_end = " » Классифицировано %d %s\n",
        .l_fm_cr_start = " » Очистка %s ...\n",
        .l_fm_cr_end = " » Очищено %d %s\n",
        .l_fm_min_size_error = " » Предупреждение: MIN больше MAX в конфигурации %s, MIN проигнорирован！\n",
        .l_fm_size_max_error = " » Предупреждение: неверное объявление MAX в конфигурации %s！Проигнорировано\n",
        .l_fm_size_min_error = " » Предупреждение: неверное объявление MIN в конфигурации %s！Проигнорировано\n",
        .l_fm_size_error = " » Предупреждение: неверная единица размера в конфигурации %s！Соответствующее значение проигнорировано\n",
        .l_open_freezer_successful = " » Включен нативный Android Freezer (^^)\n",
        .l_si_open_failed_storage = " » Не удалось заблокировать обновления во внутреннем хранилище: %s\n",
        .l_si_open_successful_storage = " » Обновления во внутреннем хранилище заблокированы！\n",
        .l_si_off_failed_storage = " » Не удалось отключить блокировку обновлений во внутреннем хранилище: %s\n",
        .l_si_off_successful_storage = " » Блокировка обновлений во внутреннем хранилище отключена！\n",
        .l_ss_open_stop_storage_successful = " » Фиксация внутреннего хранилища включена！\n",
        .l_ss_open_stop_storage_failed = " » Не удалось включить фиксацию внутреннего хранилища: %s\n",
        .l_ss_off_stop_storage_successful = " » Фиксация внутреннего хранилища отключена！\n",
        .l_ss_off_stop_storage_failed = " » Не удалось отключить фиксацию внутреннего хранилища: %s\n",
        .l_sc_successful_storage = " » Очистка мусора во внутреннем хранилище завершена！\n » Очищено %d пустых файлов/папок и Log-файлов\n",
        .l_sc_successful_sd = " » Очистка мусора во внешнем хранилище %s завершена！\n » Очищено %d пустых файлов/папок и Log-файлов\n",
        .l_sc_clear = " » Очистка кэша хранилища %s\n",
        .l_sc_clear_err = " » Не удалось очистить кэш хранилища %s！\n",
        .l_sc_clear_cache_done = " » Очищено кэшей %d приложений！\n",
        .l_sc_clear_dirty_err = " » Не удалось очистить пустые файлы/папки и Log-файлы！\n",
        .l_sr_start = " » Обработка конфигурации %s📍\n",
        .l_sr_w_config_stattpath_err = " » Предупреждение: начальный путь, указанный в конфигурации %s, не существует！\n",
        .l_sr_clear_successful = " » Очистка %s выполнена успешно！%.2f %c\n",
        .l_sr_clear_failed = " » Не удалось очистить %s\n",
        .l_sr_line_failed_path_err = " » Ошибка в строке %d: неверный путь / недоступен\n",
        .l_sr_end = " » Обработка пользовательских каталогов завершена！\n",
        .l_bm_mount_success = " » Каталог %s успешно привязан к %s\n",
        .l_bm_error_is_sdcardfs = " » Ваше устройство использует sdcardfs, эта функция не может работать корректно！\n",
    },
    [LANG_en_US] =
    {
        .l_lang_name = "en_US",
        
        .l_not_use_root = " » Please use Root permissions! #%u\n",
        .l_args_failed = " » Insufficient arguments provided!\n",
        .l_mode_err = " » Unknown mode! [%s]\n",
        .l_noconfig = " » No configuration found!\n",
        .l_open_path_failed = " » Failed to open directory %s: %s\n",
        .l_open_file_failed = " » Failed to open file %s: %s\n",
        .l_mkdir_error = " » Failed to create directory %s: %s\n",
        .l_mount_error = " » Failed to mount %s to %s: %s\n",
        .l_get_applist_error = " » Failed to get application list!\n",
        .l_get_sys_version_err = " » Failed to get system version!\n",
        .l_w_setprop_err = " » Warning: PROP setting failed!\n",
        .l_move_error = " » Failed to move %s: %s\n",
        .l_delete_error = " » Failed to delete %s: %s\n",
        .l_server_start_err = " » Process startup failed: %s\n",
        .l_path_file_error = " » PATH configuration error, automatically rewriting ... Please try again. If it fails, please delete %s and update the configuration before retrying\n",
        
        .l_clear_all = "One-click optimize & cleanup\n",
        .l_clear_cache = "Clear app cache\n",
        .l_clear_sys_cache = "Clear system cache\n",
        .l_cust_storage_clean = "Custom storage rule cleanup\n",
        .l_storage_clean = "Storage directory cleanup\n",
        .l_file_clean = "File cleanup\n",
        .l_cust_app_clean = "Custom app rule cleanup\n",
        .l_app_clean = "App rule cleanup\n",
        .l_file_all = "File organization\n",
        .l_stop_install = "Block installation\n",
        .l_lock_storage = "Storage lock\n",
        .l_fast_disk_gc = "Fast disk optimization\n",
        .l_disk_gc = "Disk GC\n",
        .l_sys_dexoat = "System DEXOAT\n",
        .l_cust_dexoat = "Custom DEXOAT\n",
        .l_reset_dexoat = "Reset DEXOAT\n",
        .l_config_mg = "Config backup/restore\n",
        .l_stop_cache_daemon = "Cache blocking (Daemon)\n",
        .l_time_daemon = "Scheduled tasks (Daemon)\n",
        .l_path_bind = "Rule Directory Binding (Redirect)\n",
        .l_storage_scan = "Storage Analysis (NCDU)\n",
        
        .l_scd_start_success = " » Cache blocking has been started! #%u",
        .l_scd_getapp_err_exit = " » Process has exited, reason: GETTOPAPP ERROR\n",
        .l_td_config_error = " » %s configuration error, Skip.\n",
        .l_td_line_err_value = " » %s configuration: line %d %s corresponding value error. Ignored this line\n",
        .l_td_line_err_key = " » %s configuration: line %d %s undefined error. Ignored this line\n",
        .l_td_max_config = " » Reached configuration loading limit %d. Skipped more configurations\n",
        .l_td_config_success = " » %s configuration loaded successfully\n",
        .l_td_start_success = " » Service started successfully！A total of %d configurations loaded！#%u",
        .l_td_w_post_title_toolong = " » Warning: %s configuration notification title length exceeds limit！Truncated\n",
        .l_td_w_post_message_toolong = " » Warning: %s configuration notification message length exceeds limit！Truncated\n",
        .l_td_config_write_error = " » %s configuration write-back failed.\n",
        .l_td_watch_config_dir_error = " » Hot-reload config initialization failed: %s\n",
        .l_td_load_config_success = " » %s configuration has been updated\n",
        .l_ac_clear = " » Cleaning %s &\n",
        .l_ac_config_app_notfound = " » Application specified in %s configuration not found!\n",
        .l_ac_config_package_err = " » Declaration error in %s configuration, package name not correctly filled!\n",
        .l_ac_config_appname_err = " » Declaration error in %s configuration, application name not correctly filled!\n",
        .l_ac_config_err = " » Error in %s configuration! Please correctly fill the declaration on the first line\n",
        .l_ac_config_err_1 = " » %s configuration line %d contains a dangerous error: starts with '/' !\n",
        .l_ac_config_err_2 = " » %s configuration line %d contains an error: path traversal!\n",
        .l_ac_clear_path_err = " » Failed to clean %s!\n",
        .l_ac_clear_path_success = " » %s processed successfully! %.2f %c\n",
        .l_ac_clean_successful = " » Software rules applied successfully! Total cleaned: %.2f %c\n",
        .l_ac_clean_failed = " » Specified application configuration not found or an error occurred! #%d\n",
        .l_cc_clear_failed = " » Failed to clean internal storage application cache\n",
        .l_cc_clear_successful = " » Internal storage application cache cleaning completed\n » Total cleaned: %.2f %c\n",
        .l_cc_clear_failed_sd = " » Failed to clean external storage application cache\n",
        .l_cc_clear_successful_sd = " » External storage application cache cleaning completed\n » Total cleaned: %.2f %c\n",
        .l_cc_clear = " » %s cache cleared\n",
        .l_cc_clear_skip = " » Skipping %s\n",
        .l_cc_clear_appcache_done = " » A total of %d apps cleaned, %d apps did not require cleaning\n",
        .l_cc_clear_systemcache = " » System cache cleared! Recommend restarting the system!\n",
        .l_do_run_system = " » Performing system default Dexoat...\n",
        .l_do_run_cust = " » Running mode %s compilation...\n",
        .l_fg_err_nof2fs = " » Your device does not use the F2FS filesystem\n » Maintenance is only supported in F2FS environments!\n",
        .l_fg_err_check = " » Your device does not support the current GC function\n",
        .l_fg_dirty = " » Current dirty segments: %ld\n",
        .l_fg_free = " » Current free segments: %ld\n\n",
        .l_fg_start = " » Background GC has started, please be patient\n",
        .l_fg_err_writesysfs = " » GC startup failed! Failed to write to node!\n",
        .l_fg_err_timeout = " » GC wait timeout, waiting has been terminated!\n",
        .l_fg_end = " » GC run completed, operation has ended!\n",
        .l_fg_run_ms = " » Has been running for %d minutes %d seconds...\n",
        .l_fg_run_s = " » Has been running for %d seconds...\n",
        .l_fg_end_dirty = " » Disk dirty blocks reduced by %ld, total %.2f %c\n",
        .l_fg_end_dirty_2 = " » Disk dirty blocks increased by %ld\n » GC may still be optimizing or may not be suitable for your device!\n",
        .l_fg_fast_gc_done = " » Fast disk optimization completed. You can try more aggressive GC optimization (・∀・)\n",
        .l_fg_dirty_low = " » Your disk is in excellent condition, no optimization needed (・∀・)\n",
        .l_fm_all_failed_storage = " » Internal storage file organization failed!\n",
        .l_fm_all_successful_storage = " » Internal storage file organization successful!\n",
        .l_fm_all_failed_sd = " » External storage file organization failed!\n",
        .l_fm_all_successful_sd = " » External storage file organization successful!\n",
        .l_fm_cr_failed_storage = " » Internal storage file cleanup failed!\n",
        .l_fm_cr_successful_storage = " » Internal storage file cleanup successful!\n",
        .l_fm_cr_failed_sd = " » External storage file cleanup failed!\n",
        .l_fm_cr_successful_sd = " » External storage file cleanup successful!\n",
        .l_fm_all_start = " » Organizing %s ...\n",
        .l_fm_all_end = " » Organized %d %s(s)\n",
        .l_fm_cr_start = " » Cleaning %s ...\n",
        .l_fm_cr_end = " » Cleaned %d %s(s)\n",
        .l_fm_min_size_error = " » Warning: MIN size in %s configuration is greater than MAX, MIN has been automatically invalidated！\n",
        .l_fm_size_max_error = " » Warning: MAX declaration in %s configuration is incorrectly filled！Ignored\n",
        .l_fm_size_min_error = " » Warning: MIN declaration in %s configuration is incorrectly filled！Ignored\n",
        .l_fm_size_error = " » Warning: Size range unit in %s configuration is incorrectly filled！Corresponding value ignored\n",
        .l_open_freezer_successful = " » Android native tombstone has been opened (^^)\n",
        .l_si_open_failed_storage = " » Failed to block updates in internal storage: %s\n",
        .l_si_open_successful_storage = " » Updates have been blocked in internal storage!\n",
        .l_si_off_failed_storage = " » Failed to disable update blocking in internal storage: %s\n",
        .l_si_off_successful_storage = " » Update blocking has been disabled in internal storage!\n",
        .l_ss_open_stop_storage_successful = " » Internal storage pinning has been enabled!\n",
        .l_ss_open_stop_storage_failed = " » Failed to enable internal storage pinning: %s\n",
        .l_ss_off_stop_storage_successful = " » Internal storage pinning has been disabled!\n",
        .l_ss_off_stop_storage_failed = " » Failed to disable internal storage pinning: %s\n",
        .l_sc_successful_storage = " » Internal storage cleanup complete!\n » Total cleaned: %d empty files/folders and log files\n",
        .l_sc_successful_sd = " » External storage %s cleanup complete!\n » Total cleaned: %d empty files/folders and log files\n",
        .l_sc_clear = " » Cleaning %s storage cache\n",
        .l_sc_clear_err = " » Failed to clean %s storage cache!\n",
        .l_sc_clear_cache_done = " » Total cleaned: %d app caches!\n",
        .l_sc_clear_dirty_err = " » Failed to clean empty files/folders and log files!\n",
        .l_sr_start = " » Processing %s configuration📍\n",
        .l_sr_w_config_stattpath_err = " » Warning: Starting path specified in %s configuration does not exist!\n",
        .l_sr_clear_successful = " » Cleaned %s successfully! %.2f %c\n",
        .l_sr_clear_failed = " » Failed to clean %s\n",
        .l_sr_line_failed_path_err = " » Line %d error: Path error/inaccessible\n",
        .l_sr_end = " » Custom directory processing completed!\n",
        .l_bm_mount_success = " » Successfully bound directory %s to %s\n",
        .l_bm_error_is_sdcardfs = " » Your device uses the sdcardfs solution, this feature cannot function properly！\n",
    },
    [LANG_ko_KR] =
    {
        .l_lang_name = "ko_KR",
        
        .l_not_use_root = " » 루트 권한을 사용하세요！#%u\n",
        .l_args_failed = " » 전달된 인수가 부족합니다！\n",
        .l_mode_err = " » 알 수 없는 모드！[%s]\n",
        .l_noconfig = " » 구성을 찾을 수 없습니다！\n",
        .l_open_path_failed = " » %s 디렉터리 열기 실패: %s\n",
        .l_open_file_failed = " » %s 파일 열기 실패: %s\n",
        .l_mkdir_error = " » %s 디렉터리 생성 실패: %s\n",
        .l_mount_error = " » %s을(를) %s에 마운트 실패: %s\n",
        .l_get_applist_error = " » 앱 목록 가져오기 실패！\n",
        .l_get_sys_version_err = " » 시스템 버전 가져오기 실패！\n",
        .l_w_setprop_err = " » 경고: PROP 설정 실패！\n",
        .l_move_error = " » %s 이동 실패: %s\n",
        .l_delete_error = " » %s 삭제 실패: %s\n",
        .l_server_start_err = " » 프로세스 시작 실패: %s\n",
        .l_path_file_error = " » PATH 구성 오류, 자동 재작성 중... 다시 시도하세요. 실패 시 %s을(를) 삭제한 후 구성을 업데이트하세요\n",
        
        .l_clear_all = "원클릭 최적화 및 정리\n",
        .l_clear_cache = "앱 캐시 정리\n",
        .l_clear_sys_cache = "시스템 캐시 정리\n",
        .l_cust_storage_clean = "사용자 정의 규칙 저장소 정리\n",
        .l_storage_clean = "저장소 디렉터리 정리\n",
        .l_file_clean = "파일 정리\n",
        .l_cust_app_clean = "사용자 정의 앱 규칙 정리\n",
        .l_app_clean = "앱 규칙 정리\n",
        .l_file_all = "파일 분류\n",
        .l_stop_install = "설치 차단\n",
        .l_lock_storage = "저장소 잠금\n",
        .l_fast_disk_gc = "빠른 디스크 최적화\n",
        .l_disk_gc = "디스크 GC\n",
        .l_sys_dexoat = "시스템 DEXOAT\n",
        .l_cust_dexoat = "사용자 정의 DEXOAT\n",
        .l_reset_dexoat = "DEXOAT 리셋\n",
        .l_config_mg = "구성 백업/복원\n",
        .l_stop_cache_daemon = "캐시 차단 (데몬)\n",
        .l_time_daemon = "자동 작업 (데몬)\n",
        .l_path_bind = "규칙 디렉터리 바인딩 (리디렉션)\n",
        .l_storage_scan = "저장소 분석 (NCDU)\n",
        
        .l_scd_start_success = " » 캐시 차단이 시작되었습니다！#%u",
        .l_scd_getapp_err_exit = " » 프로세스가 종료되었습니다. 이유: GETTOPAPP ERROR\n",
        .l_td_config_error = " » %s 구성 오류, 건너뜁니다.\n",
        .l_td_line_err_value = " » %s 구성: %d행 %s 값 오류. 해당 행을 무시합니다\n",
        .l_td_line_err_key = " » %s 구성: %d행 %s 정의되지 않은 오류. 해당 행을 무시합니다\n",
        .l_td_max_config = " » 구성 로드 한도 %d에 도달했습니다. 추가 구성은 건너뜁니다\n",
        .l_td_config_success = " » %s 구성 로드 성공\n",
        .l_td_start_success = " » 서비스가 성공적으로 시작되었습니다! 총 %d개의 구성이 로드되었습니다！#%u",
        .l_td_w_post_title_toolong = " » 경고: %s 구성 알림 제목이 길이 제한을 초과했습니다! 잘렸습니다\n",
        .l_td_w_post_message_toolong = " » 경고: %s 구성 알림 내용이 길이 제한을 초과했습니다! 잘렸습니다\n",
        .l_td_config_write_error = " » %s 구성 쓰기 실패.\n",
        .l_td_watch_config_dir_error = " » 핫 업데이트 구성 초기화 실패: %s\n",
        .l_td_load_config_success = " » %s 구성이 업데이트되었습니다\n",
        .l_ac_clear = " » %s 정리 중 &\n",
        .l_ac_config_app_notfound = " » %s 구성에 지정된 앱을 찾을 수 없습니다！\n",
        .l_ac_config_package_err = " » %s 구성 선언 오류: 패키지 이름이 올바르지 않습니다！\n",
        .l_ac_config_appname_err = " » %s 구성 선언 오류: 앱 이름이 올바르지 않습니다！\n",
        .l_ac_config_err = " » %s 구성 오류! 첫 번째 줄에 선언을 올바르게 작성하세요\n",
        .l_ac_config_err_1 = " » %s 구성 %d행에 위험한 오류가 있습니다: \"/\"로 시작됨！\n",
        .l_ac_config_err_2 = " » %s 구성 %d행에 오류가 있습니다: 경로 이스케이프！\n",
        .l_ac_clear_path_err = " » %s 정리 실패！\n",
        .l_ac_clear_path_success = " » %s 처리 성공！%.2f %c\n",
        .l_ac_clean_successful = " » 앱 규칙 처리 성공！총 정리: %.2f %c\n",
        .l_ac_clean_failed = " » 지정된 앱 구성을 찾을 수 없거나 오류 발생！#%d\n",
        .l_cc_clear_failed = " » 내부 저장소 앱 캐시 정리 실패\n",
        .l_cc_clear_successful = " » 내부 저장소 앱 캐시 정리 완료\n » 총 정리: %.2f %c\n",
        .l_cc_clear_failed_sd = " » 외부 저장소 앱 캐시 정리 실패\n",
        .l_cc_clear_successful_sd = " » 외부 저장소 앱 캐시 정리 완료\n » 총 정리: %.2f %c\n",
        .l_cc_clear = " » %s 캐시가 지워졌습니다\n",
        .l_cc_clear_skip = " » %s 건너뜀\n",
        .l_cc_clear_appcache_done = " » 총 %d개의 앱을 정리했으며, %d개의 앱은 정리가 필요하지 않았습니다\n",
        .l_cc_clear_systemcache = " » 시스템 캐시가 비워졌습니다! 시스템 재부팅을 권장합니다！\n",
        .l_do_run_system = " » 시스템 기본 Dexoat를 실행 중입니다...\n",
        .l_do_run_cust = " » %s 모드로 컴파일 중입니다...\n",
        .l_fg_err_nof2fs = " » 기기가 F2FS 파일 시스템이 아닙니다\n » 유지보수는 F2FS 환경만 지원합니다！\n",
        .l_fg_err_check = " » 기기가 현재 GC 기능을 지원하지 않습니다\n",
        .l_fg_dirty = " » 현재 더티 세그먼트: %ld\n",
        .l_fg_free = " » 현재 여유 세그먼트: %ld\n\n",
        .l_fg_start = " » 백그라운드 GC가 시작되었습니다. 기다려주세요\n",
        .l_fg_err_writesysfs = " » GC 시작 실패! 노드 쓰기 실패！\n",
        .l_fg_err_timeout = " » GC 대기 시간 초과, 대기 종료！\n",
        .l_fg_end = " » GC 실행 완료, 실행 종료！\n",
        .l_fg_run_ms = " » 실행 시간 %d분 %d초...\n",
        .l_fg_run_s = " » 실행 시간 %d초...\n",
        .l_fg_end_dirty = " » 디스크 더티 블록이 %ld 감소, 총 %.2f %c\n",
        .l_fg_end_dirty_2 = " » 디스크 더티 블록이 %ld 증가\n » GC가 아직 최적화 중이거나 기기에 적합하지 않을 수 있습니다！\n",
        .l_fg_fast_gc_done = " » 빠른 디스크 최적화 완료, 더 공격적인 GC 최적화를 시도해보세요 (・∀・)\n",
        .l_fg_dirty_low = " » 디스크 상태가 매우 좋습니다. 최적화가 필요하지 않습니다 (・∀・)\n",
        .l_fm_all_failed_storage = " » 내부 저장소 파일 분류 실패！\n",
        .l_fm_all_successful_storage = " » 내부 저장소 파일 분류 성공！\n",
        .l_fm_all_failed_sd = " » 외부 저장소 파일 분류 실패！\n",
        .l_fm_all_successful_sd = " » 외부 저장소 파일 분류 성공！\n",
        .l_fm_cr_failed_storage = " » 내부 저장소 파일 정리 실패！\n",
        .l_fm_cr_successful_storage = " » 내부 저장소 파일 정리 성공！\n",
        .l_fm_cr_failed_sd = " » 외부 저장소 파일 정리 실패！\n",
        .l_fm_cr_successful_sd = " » 외부 저장소 파일 정리 성공！\n",
        .l_fm_all_start = " » %s 분류 중 ...\n",
        .l_fm_all_end = " » %d개의 %s 분류 완료\n",
        .l_fm_cr_start = " » %s 정리 중 ...\n",
        .l_fm_cr_end = " » %d개의 %s 정리 완료\n",
        .l_fm_min_size_error = " » 경고: %s 구성에서 MIN 크기가 MAX보다 큽니다. MIN이 무시되었습니다！\n",
        .l_fm_size_max_error = " » 경고: %s 구성의 MAX 선언이 잘못되었습니다! 무시됨\n",
        .l_fm_size_min_error = " » 경고: %s 구성의 MIN 선언이 잘못되었습니다! 무시됨\n",
        .l_fm_size_error = " » 경고: %s 구성의 크기 범위 단위가 잘못되었습니다! 해당 값이 무시됨\n",
        .l_open_freezer_successful = " » Android 네이티브 프리저가 활성화되었습니다 (^^)\n",
        .l_si_open_failed_storage = " » 내부 저장소 업데이트 차단 실패: %s\n",
        .l_si_open_successful_storage = " » 내부 저장소 업데이트가 차단되었습니다！\n",
        .l_si_off_failed_storage = " » 내부 저장소 업데이트 차단 해제 실패: %s\n",
        .l_si_off_successful_storage = " » 내부 저장소 업데이트 차단이 해제되었습니다！\n",
        .l_ss_open_stop_storage_successful = " » 내부 저장소 고정이 활성화되었습니다！\n",
        .l_ss_open_stop_storage_failed = " » 내부 저장소 고정 활성화 실패: %s\n",
        .l_ss_off_stop_storage_successful = " » 내부 저장소 고정이 비활성화되었습니다！\n",
        .l_ss_off_stop_storage_failed = " » 내부 저장소 고정 비활성화 실패: %s\n",
        .l_sc_successful_storage = " » 내부 저장소 쓰레기 정리 완료！\n » 총 %d개의 빈 파일/폴더 및 로그 파일 정리\n",
        .l_sc_successful_sd = " » 외부 저장소 %s 쓰레기 정리 완료！\n » 총 %d개의 빈 파일/폴더 및 로그 파일 정리\n",
        .l_sc_clear = " » %s 저장소 캐시 정리\n",
        .l_sc_clear_err = " » %s 저장소 캐시 정리 실패！\n",
        .l_sc_clear_cache_done = " » 총 %d개의 앱 캐시 정리！\n",
        .l_sc_clear_dirty_err = " » 빈 파일/폴더 및 로그 파일 정리 실패！\n",
        .l_sr_start = " » %s 구성 처리 중📍\n",
        .l_sr_w_config_stattpath_err = " » 경고: %s 구성에 지정된 초기 경로가 존재하지 않습니다！\n",
        .l_sr_clear_successful = " » %s 정리 성공！%.2f %c\n",
        .l_sr_clear_failed = " » %s 정리 실패\n",
        .l_sr_line_failed_path_err = " » %d행 오류: 경로 오류/접근 불가\n",
        .l_sr_end = " » 사용자 정의 디렉터리 처리 완료！\n",
        .l_bm_mount_success = " » %s 디렉터리가 %s에 성공적으로 바인딩되었습니다\n",
        .l_bm_error_is_sdcardfs = " » 기기가 sdcardfs를 사용 중입니다. 이 기능은 정상 작동하지 않을 수 있습니다！\n",
    },
};

extern LangType current_lang;

#define L_LANG_NAME lang_packs[current_lang].l_lang_name

#define L_NOT_USE_ROOT lang_packs[current_lang].l_not_use_root
#define L_ARGS_FAILED lang_packs[current_lang].l_args_failed
#define L_MODE_ERR lang_packs[current_lang].l_mode_err
#define L_NOCONFIG lang_packs[current_lang].l_noconfig
#define L_OPEN_PATH_FAILED lang_packs[current_lang].l_open_path_failed
#define L_OPEN_FILE_FAILED lang_packs[current_lang].l_open_file_failed
#define L_MKDIR_ERROR lang_packs[current_lang].l_mkdir_error
#define L_MOUNT_ERROR lang_packs[current_lang].l_mount_error
#define L_GET_APPLIST_ERROR lang_packs[current_lang].l_get_applist_error
#define L_GET_SYS_VERSION_ERR lang_packs[current_lang].l_get_sys_version_err
#define L_W_SETPROP_ERR lang_packs[current_lang].l_w_setprop_err
#define L_MOVE_ERROR lang_packs[current_lang].l_move_error
#define L_DELETE_ERROR lang_packs[current_lang].l_delete_error
#define L_SERVER_START_ERR lang_packs[current_lang].l_server_start_err
#define L_PATH_FILE_ERROR lang_packs[current_lang].l_path_file_error

#define L_CLEAR_ALL lang_packs[current_lang].l_clear_all
#define L_CLEAR_CACHE lang_packs[current_lang].l_clear_cache
#define L_CLEAR_SYS_CACHE lang_packs[current_lang].l_clear_sys_cache
#define L_CUST_STORAGE_CLEAN lang_packs[current_lang].l_cust_storage_clean
#define L_STORAGE_CLEAN lang_packs[current_lang].l_storage_clean
#define L_FILE_CLEAN lang_packs[current_lang].l_file_clean
#define L_CUST_APP_CLEAN lang_packs[current_lang].l_cust_app_clean
#define L_APP_CLEAN lang_packs[current_lang].l_app_clean
#define L_FILE_ALL lang_packs[current_lang].l_file_all
#define L_STOP_INSTALL lang_packs[current_lang].l_stop_install
#define L_LOCK_STORAGE lang_packs[current_lang].l_lock_storage
#define L_FAST_DISK_GC lang_packs[current_lang].l_fast_disk_gc
#define L_DISK_GC lang_packs[current_lang].l_disk_gc
#define L_SYS_DEXOAT lang_packs[current_lang].l_sys_dexoat
#define L_CUST_DEXOAT lang_packs[current_lang].l_cust_dexoat
#define L_RESET_DEXOAT lang_packs[current_lang].l_reset_dexoat
#define L_CONFIG_MG lang_packs[current_lang].l_config_mg
#define L_STOP_CACHE_DAEMON lang_packs[current_lang].l_stop_cache_daemon
#define L_TIME_DAEMON lang_packs[current_lang].l_time_daemon
#define L_PATH_BIND lang_packs[current_lang].l_path_bind
#define L_STORAGE_SCAN lang_packs[current_lang].l_storage_scan

#define L_SCD_START_SUCCESS lang_packs[current_lang].l_scd_start_success
#define L_SCD_GETAPP_ERR_EXIT lang_packs[current_lang].l_scd_getapp_err_exit
#define L_TD_CONFIG_ERROR lang_packs[current_lang].l_td_config_error
#define L_TD_LINE_ERR_VALUE lang_packs[current_lang].l_td_line_err_value
#define L_TD_LINE_ERR_KEY lang_packs[current_lang].l_td_line_err_key
#define L_TD_MAX_CONFIG lang_packs[current_lang].l_td_max_config
#define L_TD_CONFIG_SUCCESS lang_packs[current_lang].l_td_config_success
#define L_TD_START_SUCCESS lang_packs[current_lang].l_td_start_success
#define L_TD_W_POST_TITLE_TOOLONG lang_packs[current_lang].l_td_w_post_title_toolong
#define L_TD_W_POST_MESSAGE_TOOLONG lang_packs[current_lang].l_td_w_post_message_toolong
#define L_TD_CONFIG_WRITE_ERROR lang_packs[current_lang].l_td_config_write_error
#define L_TD_WATCH_CONFIG_DIR_ERROR lang_packs[current_lang].l_td_watch_config_dir_error
#define L_TD_LOAD_CONFIG_SUCCESS lang_packs[current_lang].l_td_load_config_success
#define L_AC_CLEAR lang_packs[current_lang].l_ac_clear
#define L_AC_CONFIG_APP_NOTFOUND lang_packs[current_lang].l_ac_config_app_notfound
#define L_AC_CONFIG_PACKAGE_ERR lang_packs[current_lang].l_ac_config_package_err
#define L_AC_CONFIG_APPNAME_ERR lang_packs[current_lang].l_ac_config_appname_err
#define L_AC_CONFIG_ERR lang_packs[current_lang].l_ac_config_err
#define L_AC_CONFIG_ERR_1 lang_packs[current_lang].l_ac_config_err_1
#define L_AC_CONFIG_ERR_2 lang_packs[current_lang].l_ac_config_err_2
#define L_AC_CLEAR_PATH_ERR lang_packs[current_lang].l_ac_clear_path_err
#define L_AC_CLEAR_PATH_SUCCESS lang_packs[current_lang].l_ac_clear_path_success
#define L_AC_CLEAN_SUCCESSFUL lang_packs[current_lang].l_ac_clean_successful
#define L_AC_CLEAN_FAILED lang_packs[current_lang].l_ac_clean_failed
#define L_CC_CLEAR_FAILED lang_packs[current_lang].l_cc_clear_failed
#define L_CC_CLEAR_SUCCESSFUL lang_packs[current_lang].l_cc_clear_successful
#define L_CC_CLEAR_FAILED_SD lang_packs[current_lang].l_cc_clear_failed_sd
#define L_CC_CLEAR_SUCCESSFUL_SD lang_packs[current_lang].l_cc_clear_successful_sd
#define L_CC_CLEAR lang_packs[current_lang].l_cc_clear
#define L_CC_CLEAR_SKIP lang_packs[current_lang].l_cc_clear_skip
#define L_CC_CLEAR_APPCACHE_DONE lang_packs[current_lang].l_cc_clear_appcache_done
#define L_CC_CLEAR_SYSTEMCACHE lang_packs[current_lang].l_cc_clear_systemcache
#define L_DO_RUN_SYSTEM lang_packs[current_lang].l_do_run_system
#define L_DO_RUN_CUST lang_packs[current_lang].l_do_run_cust
#define L_FG_ERR_NOF2FS lang_packs[current_lang].l_fg_err_nof2fs
#define L_FG_ERR_CHECK lang_packs[current_lang].l_fg_err_check
#define L_FG_DIRTY lang_packs[current_lang].l_fg_dirty
#define L_FG_FREE lang_packs[current_lang].l_fg_free
#define L_FG_START lang_packs[current_lang].l_fg_start
#define L_FG_ERR_WRITESYSFS lang_packs[current_lang].l_fg_err_writesysfs
#define L_FG_ERR_TIMEOUT lang_packs[current_lang].l_fg_err_timeout
#define L_FG_END lang_packs[current_lang].l_fg_end
#define L_FG_RUN_MS lang_packs[current_lang].l_fg_run_ms
#define L_FG_RUN_S lang_packs[current_lang].l_fg_run_s
#define L_FG_END_DIRTY lang_packs[current_lang].l_fg_end_dirty
#define L_FG_END_DIRTY_2 lang_packs[current_lang].l_fg_end_dirty_2
#define L_FG_FAST_GC_DONE lang_packs[current_lang].l_fg_fast_gc_done
#define L_FG_DIRTY_LOW lang_packs[current_lang].l_fg_dirty_low
#define L_FM_ALL_FAILED_STORAGE lang_packs[current_lang].l_fm_all_failed_storage
#define L_FM_ALL_SUCCESSFUL_STORAGE lang_packs[current_lang].l_fm_all_successful_storage
#define L_FM_ALL_FAILED_SD lang_packs[current_lang].l_fm_all_failed_sd
#define L_FM_ALL_SUCCESSFUL_SD lang_packs[current_lang].l_fm_all_successful_sd
#define L_FM_CR_FAILED_STORAGE lang_packs[current_lang].l_fm_cr_failed_storage
#define L_FM_CR_SUCCESSFUL_STORAGE lang_packs[current_lang].l_fm_cr_successful_storage
#define L_FM_CR_FAILED_SD lang_packs[current_lang].l_fm_cr_failed_sd
#define L_FM_CR_SUCCESSFUL_SD lang_packs[current_lang].l_fm_cr_successful_sd
#define L_FM_ALL_START lang_packs[current_lang].l_fm_all_start
#define L_FM_ALL_END lang_packs[current_lang].l_fm_all_end
#define L_FM_CR_START lang_packs[current_lang].l_fm_cr_start
#define L_FM_CR_END lang_packs[current_lang].l_fm_cr_end
#define L_FM_MIN_SIZE_ERROR lang_packs[current_lang].l_fm_min_size_error
#define L_FM_SIZE_MAX_ERROR lang_packs[current_lang].l_fm_size_max_error
#define L_FM_SIZE_MIN_ERROR lang_packs[current_lang].l_fm_size_min_error
#define L_FM_SIZE_ERROR lang_packs[current_lang].l_fm_size_error
#define L_OPEN_FREEZER_SUCCESSFUL lang_packs[current_lang].l_open_freezer_successful
#define L_SI_OPEN_FAILED_STORAGE lang_packs[current_lang].l_si_open_failed_storage
#define L_SI_OPEN_SUCCESSFUL_STORAGE lang_packs[current_lang].l_si_open_successful_storage
#define L_SI_OFF_FAILED_STORAGE lang_packs[current_lang].l_si_off_failed_storage
#define L_SI_OFF_SUCCESSFUL_STORAGE lang_packs[current_lang].l_si_off_successful_storage
#define L_SS_OPEN_STOP_STORAGE_SUCCESSFUL lang_packs[current_lang].l_ss_open_stop_storage_successful
#define L_SS_OPEN_STOP_STORAGE_FAILED lang_packs[current_lang].l_ss_open_stop_storage_failed
#define L_SS_OFF_STOP_STORAGE_SUCCESSFUL lang_packs[current_lang].l_ss_off_stop_storage_successful
#define L_SS_OFF_STOP_STORAGE_FAILED lang_packs[current_lang].l_ss_off_stop_storage_failed
#define L_SC_SUCCESSFUL_STORAGE lang_packs[current_lang].l_sc_successful_storage
#define L_SC_SUCCESSFUL_SD lang_packs[current_lang].l_sc_successful_sd
#define L_SC_CLEAR lang_packs[current_lang].l_sc_clear
#define L_SC_CLEAR_ERR lang_packs[current_lang].l_sc_clear_err
#define L_SC_CLEAR_CACHE_DONE lang_packs[current_lang].l_sc_clear_cache_done
#define L_SC_CLEAR_DIRTY_ERR lang_packs[current_lang].l_sc_clear_dirty_err
#define L_SR_START lang_packs[current_lang].l_sr_start
#define L_SR_W_CONFIG_STATTPATH_ERR lang_packs[current_lang].l_sr_w_config_stattpath_err
#define L_SR_CLEAR_SUCCESSFUL lang_packs[current_lang].l_sr_clear_successful
#define L_SR_CLEAR_FAILED lang_packs[current_lang].l_sr_clear_failed
#define L_SR_LINE_FAILED_PATH_ERR lang_packs[current_lang].l_sr_line_failed_path_err
#define L_SR_END lang_packs[current_lang].l_sr_end
#define L_BM_MOUNT_SUCCESS lang_packs[current_lang].l_bm_mount_success
#define L_BM_ERROR_IS_SDCARDFS lang_packs[current_lang].l_bm_error_is_sdcardfs
