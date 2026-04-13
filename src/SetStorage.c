// 此Code来自ClearBox模块，用于内部储存固定功能
#include "INCLUDE/BashCore.h"

#define DATA_DIR "/data/media/0"
#define STOP_COMMAND "%s/busybox chattr +i %s 2>/dev/null" //Max Size 62
#define RESET_COMMAND "%s/busybox chattr -i %s 2>/dev/null" //Max Size 62
#define PROP_STOP "clearbox_stop_storage=1"
#define PROP_RESET "clearbox_stop_storage=0"

int set_storage(char * work_dir, char * bin_dir, char * mode)
{
    // Stop & Reset
    char command[strlen(bin_dir) + strlen(DATA_DIR) + 64];
    
    if (strcasecmp(mode, "STOP") == 0)
    {
        snprintf(command, sizeof(command), STOP_COMMAND, bin_dir, DATA_DIR);
        if (system(command) == 0)
        {
            printf(L_SS_OPEN_STOP_STORAGE_SUCCESSFUL);
        }
        else
        {
            fprintf(stderr, L_SS_OPEN_STOP_STORAGE_FAILED);
            return 1;
        }
        fflush(stdout);
    }
    else if (strcasecmp(mode, "RESET") == 0)
    {
        snprintf(command, sizeof(command), RESET_COMMAND, bin_dir, DATA_DIR);
        if (system(command) == 0)
        {
            printf(L_SS_OFF_STOP_STORAGE_SUCCESSFUL);
        }
        else
        {
            fprintf(stderr, L_SS_OFF_STOP_STORAGE_FAILED);
            return 1;
        }
        fflush(stdout);
    }
    else
    {
        fprintf(stderr, L_MODE_ERR, mode);
        return 1;
    }
    
    int success = 0;
    char settings_file[strlen(work_dir) + sizeof(SETTINGS_FILE) + 2];
    snprintf(settings_file, sizeof(settings_file), "%s/%s", work_dir, SETTINGS_FILE);
    
    if (strcasecmp(mode, "STOP") == 0)
    {
        success = s_sed(settings_file, PROP_RESET, PROP_STOP);
    }
    else
    {
        success = s_sed(settings_file, PROP_STOP, PROP_RESET);
    }
    if (success != 0)
    {
        fprintf(stderr, L_W_SETPROP_ERR);
    }
    
    return 0;
}
