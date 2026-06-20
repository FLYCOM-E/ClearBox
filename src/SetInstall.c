/*
                    GNU GENERAL PUBLIC
                        Version 3

         此 Code 来自 ClearBox 模块，用于阻止安装功能
*/

#include "INCLUDE/main.h"

#define DATA_DIR "/data/app"                    // 阻止目录
#define PROP_STOP "clearbox_stop_install=1"     // PROP OPEN
#define PROP_RESET "clearbox_stop_install=0"    // PROP CLOSE

int set_install(char * mode)
{
    // Stop & Reset
    int success = 0;
    
    if (strcasecmp(mode, "STOP") == 0)
    {
        if (s_chattr(DATA_DIR, 1, 0) == 0)
        {
            printf(L_SI_OPEN_SUCCESSFUL_STORAGE);
            success = s_sed(settings_file, PROP_RESET, PROP_STOP, 0);
        }
        else
        {
            fprintf(stderr, L_SI_OPEN_FAILED_STORAGE, strerror(errno));
        }
    }
    else if (strcasecmp(mode, "RESET") == 0)
    {
        if (s_chattr(DATA_DIR, 0, 0) == 0)
        {
            printf(L_SI_OFF_SUCCESSFUL_STORAGE);
            success = s_sed(settings_file, PROP_STOP, PROP_RESET, 0);
        }
        else
        {
            fprintf(stderr, L_SI_OFF_FAILED_STORAGE, strerror(errno));
        }
    }
    else
    {
        fprintf(stderr, L_MODE_ERR, mode);
        return -1;
    }
    fflush(stdout);
    
    if (success != 0)
    {
        fprintf(stderr, L_W_SETPROP_ERR);
    }
    
    return 0;
}
