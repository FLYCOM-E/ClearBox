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

#define MAX_PACKAGE 256
#define MAX_PATH 4096

#ifdef zh_CN
    #include "LANG/zh_CN.h"
#elif defined(en_US)
    #include "LANG/en_US.h"
#elif defined(zh_TW)
    #include "LANG/zh_TW.h"
#else
    #include "LANG/zh_CN.h"
#endif
