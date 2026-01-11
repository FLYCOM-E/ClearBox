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

#include <android/log.h>
#include <sys/system_properties.h>

#define MAX_PACKAGE 256
#define MAX_PATH 4096
#define MAX_WORK_DIR_LEN 512
#define MAX_BIN_DIR_LEN 512

long s_remove(char * path, int all);
int whiteListCheck(char * whitelist_file, char * package);
long GetPathSize(char * path);
int post(char * title, char * message);

#ifdef zh_CN
    #include "LANG/zh_CN.h"
#elif defined(en_US)
    #include "LANG/en_US.h"
#elif defined(zh_TW)
    #include "LANG/zh_TW.h"
#else
    #include "LANG/zh_CN.h"
#endif
