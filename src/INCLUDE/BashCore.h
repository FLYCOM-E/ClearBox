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
#include <sys/prctl.h>
#include <android/log.h>
#include <sys/system_properties.h>
#include <linux/fs.h>
#include <signal.h>
#include <sys/mount.h>

// 多语言
#include "Lang.h"

#define MAX_PACKAGE 256                 // 软件包名最大长度
#define MAX_PATH 4096                   // 文件路径最大长度
#define MAX_WORK_DIR_LEN 512            // 工作目录最大长度
#define MAX_BIN_DIR_LEN 512             // 二进制目录最大长度
#define LOG_FILE_NAME "LOG.log"         // 日志文件名 Max Size 30
#define SETTINGS_FILE "settings.prop"   // 设置信息文件名 Max Size 30
#define SETTINGS_FILE_MAX_LINE 256      // 设置信息文件最大行长
#define getprop __system_property_get
#define print_log __android_log_print
extern volatile sig_atomic_t sig_flag;

long s_remove(char * path, int all);
int whitelist_check(char * whitelist_file, char * package);
long get_path_size(char * path);
int post(const char * title, const char * message, const char * id);
int write_log(char * config_dir, char * name_id, char * text);
int set_name_space(void);
int get_settings_prop(char * settings_file, char * key);
int set_server_name(char * argv[], char * new_name);
int s_sed(char * file, char * target_line, char * text, int mode);
int s_chattr(char * path, int mode, int dir);
int help(char * argv[]);
int s_daemon(void);
int s_signed(void);

int app_cache_clean(char * work_dir, int mode);
int cust_rule_clean(char * work_dir);
int storage_clean(char * work_dir);
int file_manager(char * work_dir, int mode, char * config_name);
int app_cust_rule_clean(char * work_dir, char * app_package, int mode);
int set_install(char * work_dir, char * mode);
int set_storage(char * work_dir, char * mode);
int disk_gc(char * argv[], int mode);
int dexoat(int mode, char * cust_mode);
int freezer_open(void);
int stop_cache_daemon(char * argv[], char * work_dir);
int time_daemon(char * argv[], char * work_dir);
int bmount(char * work_dir);
