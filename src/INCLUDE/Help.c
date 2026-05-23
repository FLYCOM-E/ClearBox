#include "BashCore.h"

int help(char * argv[])
{
    printf(" Usage: %s <args>\n", argv[0]);
    printf(" \tClearAll\t%s", L_CLEAR_ALL);
    printf(" \tClearCache\t%s", L_CLEAR_CACHE);
    printf(" \tClear_SCache\t%s", L_CLEAR_SYS_CACHE);
    printf(" \tList_Dir\t%s", L_CUST_STORAGE_CLEAN);
    printf(" \tAll_Dir\t\t%s", L_STORAGE_CLEAN);
    printf(" \tFile_Clear\t%s", L_FILE_CLEAN);
    printf(" \tClear_App\t%s", L_CUST_APP_CLEAN);
    printf(" \tClear_App_All\t%s", L_APP_CLEAN);
    printf(" \tFile_All\t%s", L_FILE_ALL);
    printf(" \tStopInstall\t%s", L_STOP_INSTALL);
    printf(" \tStopStorage\t%s", L_LOCK_STORAGE);
    printf(" \tFast_GC\t\t%s", L_FAST_DISK_GC);
    printf(" \tF2fs_GC\t\t%s", L_DISK_GC);
    printf(" \tDexoat_1\t%s", L_SYS_DEXOAT);
    printf(" \tDexoat_2\t%s", L_CUST_DEXOAT);
    printf(" \tDexoat_3\t%s", L_RESET_DEXOAT);
    printf(" \tconfigManager\t%s", L_CONFIG_MG);
    printf(" \tStopCached\t%s", L_STOP_CACHE_DAEMON);
    printf(" \tTimed\t\t%s", L_TIME_DAEMON);
    printf(" \tBindPath\t%s", L_PATH_BIND);
    
    return 0;
}
