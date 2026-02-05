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

#define MAX_PACKAGE 256
#define MAX_PATH 4096
#define MAX_WORK_DIR_LEN 512
#define MAX_BIN_DIR_LEN 512
#define LOG_FILE_NAME "LOG.log" //Max Size 30

long s_remove(char * path, int all);
int whitelist_check(char * whitelist_file, char * package);
long get_path_size(char * path);
int post(char * title, char * message);
int write_log(char * config_dir, char * name_id, char * text);
int set_name_space();

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
