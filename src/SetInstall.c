// 此Code来自ClearBox模块，用于阻止安装功能
#include "INCLUDE/BashCore.h"

#define DATA_DIR "/data/app"
#define STOP_INSTALL "%s/busybox chattr +i %s 2>/dev/null" //Max Size 60
#define RESET_INSTALL "%s/busybox chattr -i %s 2>/dev/null" //Max Size 60
#define SETPROP_STOP "sed -i 's/clearbox_stop_install=0/clearbox_stop_install=1/g' %s/settings.prop" //Max Size 126
#define SETPROP_RESET "sed -i 's/clearbox_stop_install=1/clearbox_stop_install=0/g' %s/settings.prop" //Max Size 126

int set_install(char * work_dir, char * bin_dir, char * mode)
{
    // Stop & Reset
    char command_data[strlen(bin_dir) + strlen(DATA_DIR) + 64];
    
    if (strcasecmp(mode, "STOP") == 0)
    {
        snprintf(command_data, sizeof(command_data), STOP_INSTALL, bin_dir, DATA_DIR);
        if (system(command_data) == 0)
        {
            printf(L_SI_OPEN_SUCCESSFUL_STORAGE);
        }
        else
        {
            fprintf(stderr, L_SI_OPEN_FAILED_STORAGE);
        }
        fflush(stdout);
    }
    else if (strcasecmp(mode, "RESET") == 0)
    {
        snprintf(command_data, sizeof(command_data), RESET_INSTALL, bin_dir, DATA_DIR);
        if (system(command_data) == 0)
        {
            printf(L_SI_OFF_SUCCESSFUL_STORAGE);
        }
        else
        {
            fprintf(stderr, L_SI_OFF_FAILED_STORAGE);
        }
        fflush(stdout);
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
