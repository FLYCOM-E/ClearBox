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
#include <sys/inotify.h>
#include <limits.h>

// 多语言
#include "Lang.h"

#define MAX_WORK_DIR_LEN 512            // 工作目录最大长度
#define MAX_BIN_DIR_LEN 512              // 二进制目录最大长度
#define LOG_FILE_NAME "LOG.log"           // 日志文件名
#define SETTINGS_FILE "settings.prop"       // 设置信息文件名
#define SETTINGS_FILE_MAX_LINE 256      // 设置信息文件最大行长
#define getprop __system_property_get
#define print_log __android_log_print

extern volatile sig_atomic_t sig_flag;
extern char home_dir[128],
             work_dir[128],
             settings_file[PATH_MAX];

long s_remove(char * path, int all);
int s_grep(char * file, char * text, int mode);
long get_path_size(char * path);
int post(const char * id, const char * title, const char * message, ...);
void write_log(const char * config_dir, const char * name_id, const char * text, ...);
int set_name_space(void);
int get_settings_prop(char * settings_file, char * key, char * str, size_t str_len);
void set_server_name(char * argv[], char * new_name);
int s_sed(char * file, char * target_line, char * text, int mode);
int s_chattr(char * path, int mode, int dir);
int help(char * argv[]);
int s_daemon(void);
int s_signal(void);
double byte_to_size(long byte, char * unit);

int app_cache_clean(int mode);
int cust_rule_clean(void);
int storage_clean(void);
int file_manager(int mode, char * config_name);
int app_cust_rule_clean(char * app_package, int mode);
int set_install(char * mode);
int set_storage(char * mode);
int disk_gc(char * argv[], int mode);
int dexoat(int mode, char * cust_mode);
int freezer_open(void);
int stop_cache_daemon(char * argv[]);
int time_daemon(char * argv[]);
int bmount(char * mode_str);
