// 此Code来自ClearBox模块，用于打开原生安卓墓碑模式（其实Shell可能更适合？
#include "INCLUDE/BashCore.h"

#define getprop __system_property_get
#define PROP "ro.build.version.sdk"
#define SET_FREEZER "device_config put activity_manager_native_boot use_freezer true"

int main()
{
    if (getuid() != 0)
    {
        printf(L_NOT_USE_ROOT);
        return 1;
    }
    
    int sdk = 0;
    char sdk_str[PROP_VALUE_MAX] = {0};
    
    if (getprop(PROP, sdk_str) <= 0)
    {
        printf(L_GET_SYSTEM_VERSION_ERR);
        return 1;
    }
    else
    {
        sdk = atoi(sdk_str);
        if (sdk >= 30)
        {
            if (system(SET_FREEZER) == 0)
            {
                printf(L_OPEN_FREEZER_SUCCESSFUL);
            }
        }
    }
    return 0;
}
