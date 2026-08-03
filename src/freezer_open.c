/*
                    GNU GENERAL PUBLIC
                        Version 3

     此 Code 来自 ClearBox 模块，用于打开原生安卓墓碑模式
                （其实 Shell 可能更适合？）
*/

#include "INCLUDE/main.h"

#define FREEZER_MIN_API 30

#define SET_FREEZER "device_config put activity_manager_native_boot use_freezer true"

#define PROP "ro.build.version.sdk"

int freezer_open(void)
{
    long sdk = 0;
    char sdk_str[PROP_VALUE_MAX] = {0};
    
    if (getprop(PROP, sdk_str) <= 0)
    {
        fprintf(stderr, L_GET_SYS_VERSION_ERR);
        return -1;
    }
    else
    {
        sdk = strtol(sdk_str, NULL, 10);
        if (sdk >= FREEZER_MIN_API)
        {
            if (system(SET_FREEZER) == 0)
            {
                printf(L_OPEN_FREEZER_SUCCESSFUL);
            }
        }
    }
    return 0;
}
