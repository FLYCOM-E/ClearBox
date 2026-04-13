// 此Code来自ClearBox模块，用于阻止安装功能
#include "INCLUDE/BashCore.h"

#define DATA_DIR "/data/app"
#define STOP_INSTALL "%s/busybox chattr +i %s 2>/dev/null" //Max Size 60
#define RESET_INSTALL "%s/busybox chattr -i %s 2>/dev/null" //Max Size 60
#define PROP_STOP "clearbox_stop_install=1"
#define PROP_RESET "clearbox_stop_install=0"

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
