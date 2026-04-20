/*
                    GNU GENERAL PUBLIC
                        Version 3

      此 Code 来自 ClearBox 模块，用于内部储存固定功能
*/

#include "INCLUDE/BashCore.h"

#define DATA_DIR "/data/media/0"                // 阻止目录
#define PROP_STOP "clearbox_stop_storage=1"     // PROP OPEN
#define PROP_RESET "clearbox_stop_storage=0"    // PROP CLOSE

int set_storage(char * work_dir, char * mode)
{
    // Stop & Reset
    int success = 0;
    char settings_file[strlen(work_dir) + sizeof(SETTINGS_FILE) + 2];
    snprintf(settings_file, sizeof(settings_file), "%s/%s", work_dir, SETTINGS_FILE);
    
    if (strcasecmp(mode, "STOP") == 0)
    {
        if (s_chattr(DATA_DIR, 1, 0) == 0)
        {
            printf(L_SS_OPEN_STOP_STORAGE_SUCCESSFUL);
            success = s_sed(settings_file, PROP_RESET, PROP_STOP, 0);
        }
        else
        {
            fprintf(stderr, L_SS_OPEN_STOP_STORAGE_FAILED, strerror(errno));
        }
    }
    else if (strcasecmp(mode, "RESET") == 0)
    {
        if (s_chattr(DATA_DIR, 0, 0) == 0)
        {
            printf(L_SS_OFF_STOP_STORAGE_SUCCESSFUL);
            success = s_sed(settings_file, PROP_STOP, PROP_RESET, 0);
        }
        else
        {
            fprintf(stderr, L_SS_OFF_STOP_STORAGE_FAILED, strerror(errno));
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
