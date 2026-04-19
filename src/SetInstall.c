// 此Code来自ClearBox模块，用于阻止安装功能
#include "INCLUDE/BashCore.h"

#define DATA_DIR "/data/app"
#define PROP_STOP "clearbox_stop_install=1"
#define PROP_RESET "clearbox_stop_install=0"

int set_install(char * work_dir, char * mode)
{
    // Stop & Reset
    int success = 0;
    char settings_file[strlen(work_dir) + sizeof(SETTINGS_FILE) + 2];
    snprintf(settings_file, sizeof(settings_file), "%s/%s", work_dir, SETTINGS_FILE);
    
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
        return 1;
    }
    fflush(stdout);
    
    if (success != 0)
    {
        fprintf(stderr, L_W_SETPROP_ERR);
    }
    
    return 0;
}
