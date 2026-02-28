// 此Code来自ClearBox模块，用于内部储存固定功能
#include "INCLUDE/BashCore.h"

#define DATA_DIR "/data/media/0"
#define STOP_COMMAND "%s/busybox chattr +i %s 2>/dev/null" //Max Size 62
#define RESET_COMMAND "%s/busybox chattr -i %s 2>/dev/null" //Max Size 62
#define SETPROP_STOP "sed -i 's/clearbox_stop_storage=0/clearbox_stop_storage=1/g' %s/settings.prop" //Max Size 126
#define SETPROP_RESET "sed -i 's/clearbox_stop_storage=1/clearbox_stop_storage=0/g' %s/settings.prop" //Max Size 126

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
    
    char set_prop[strlen(work_dir) + 128];
    if (strcasecmp(mode, "STOP") == 0)
    {
        snprintf(set_prop, sizeof(set_prop), SETPROP_STOP, work_dir);
    }
    else
    {
        snprintf(set_prop, sizeof(set_prop), SETPROP_RESET, work_dir);
    }
    if (system(set_prop) != 0)
    {
        fprintf(stderr, L_W_SETPROP_ERR);
    }
    
    return 0;
}
