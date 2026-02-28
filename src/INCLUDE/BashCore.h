//By ClearBox
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sched.h>

#include <android/log.h>
#include <sys/system_properties.h>

#define MAX_PACKAGE 256 // 软件包名最大长度
#define MAX_PATH 4096 // 文件路径最大长度
#define MAX_WORK_DIR_LEN 512 // 工作目录最大长度
#define MAX_BIN_DIR_LEN 512 // 二进制目录最大长度
#define LOG_FILE_NAME "LOG.log" // 日志文件名 Max Size 30
#define SETTINGS_FILE "settings.prop" // 设置信息文件名 Max Size 30
#define SETTINGS_FILE_MAX_LINE 256 // 设置信息文件最大行长
#define getprop __system_property_get

long s_remove(char * path, int all);
int whitelist_check(char * whitelist_file, char * package);
long get_path_size(char * path);
int post(char * title, char * message);
int write_log(char * config_dir, char * name_id, char * text);
int set_name_space(void);
int get_settings_prop(char * settings_file, char * key);

int app_cache_clean(char * work_dir, int mode);
int cust_rule_clean(char * work_dir);
int storage_clean(char * work_dir);
int file_manager(char * work_dir, int mode, char * config_name);
int app_cust_rule_clean(char * work_dir, char * app_package);
int set_install(char * work_dir, char * bin_dir, char * mode);
int set_storage(char * work_dir, char * bin_dir, char * mode);
int disk_gc(int mode);
int dexoat(int mode, char * cust_mode);
int freezer_open(void);

// Lang
#ifdef zh_CN
    #include "LANG/zh_CN.h"
#elif defined(en_US)
    #include "LANG/en_US.h"
#elif defined(zh_TW)
    #include "LANG/zh_TW.h"
#else
    #include "LANG/zh_CN.h"
#endif
