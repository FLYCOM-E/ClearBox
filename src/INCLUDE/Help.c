#include "main.h"

int help(char * argv[])
{
    printf(" Usage: %s <args>\n", argv[0]);
    printf(" \t--clear-all\t%s", L_CLEAR_ALL);
    printf(" \t--clear-app-cache\t%s", L_CLEAR_CACHE);
    printf(" \t--clear-system-cache\t%s", L_CLEAR_SYS_CACHE);
    printf(" \t--clear-storage-cust\t%s", L_CUST_STORAGE_CLEAN);
    printf(" \t--clear-storage\t\t%s", L_STORAGE_CLEAN);
    printf(" \t--file-clear\t%s", L_FILE_CLEAN);
    printf(" \t--clear-app-cust\t%s", L_CUST_APP_CLEAN);
    printf(" \t--clear-app-cust-all\t%s", L_APP_CLEAN);
    printf(" \t--file-sort\t%s", L_FILE_ALL);
    printf(" \t--app-allow-install\t%s", L_STOP_INSTALL);
    printf(" \t--storage-lock\t%s", L_LOCK_STORAGE);
    printf(" \t--disk-gc\t\t%s", L_FAST_DISK_GC);
    printf(" \t--disk-f2fs-gc\t\t%s", L_DISK_GC);
    printf(" \t--dexoat-system\t%s", L_SYS_DEXOAT);
    printf(" \t--dexoat-custom\t%s", L_CUST_DEXOAT);
    printf(" \t--dexoat-reset\t%s", L_RESET_DEXOAT);
    printf(" \t--config\t%s", L_CONFIG_MG);
    printf(" \t--daemon-stop-cache\t%s", L_STOP_CACHE_DAEMON);
    printf(" \t--daemon-time\t\t%s", L_TIME_DAEMON);
    printf(" \t--storage-bind-custom\t%s", L_PATH_BIND);
    
    return 0;
}
