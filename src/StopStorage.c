// 此Code来自ClearBox模块，用于内部储存固定功能
#include "BashCore.h"

#define DATA_DIR "/data/media/0"
#define STOP_COMMAND "%s/busybox chattr +i %s 2>/dev/null" //Max Size 62
#define RESET_COMMAND "%s/busybox chattr -i %s 2>/dev/null" //Max Size 62
#define SETPROP_STOP "sed -i 's/stopstorage=0/stopstorage=1/g' %s/settings.prop" //Max Size 126
#define SETPROP_RESET "sed -i 's/stopstorage=1/stopstorage=0/g' %s/settings.prop" //Max Size 126

int main(int COMI, char * COM[])
{
    if (getuid() != 0)
    {
        printf(L_NOT_USE_ROOT);
        return 1;
    }
    if (COMI < 6)
    {
        printf(L_ARGS_FAILED);
        return 1;
    }
    
    int stop = 0;
    char work_dir[128] = "", bin_dir[128] = "", mode[16] = "";
    for (int i = 0; i < COMI - 1; i++)
    {
        if (strcmp(COM[i], "-w") == 0)
        {
            if (strlen(COM[i + 1]) > 128)
            {
                printf(L_CONFIG_PATH_TOOLONG);
                return 1;
            }
            if (access(COM[i + 1], F_OK) != 0)
            {
                printf(L_CONFIG_PATH_NOTFIND);
                return 1;
            }
            snprintf(work_dir, sizeof(work_dir), "%s", COM[i + 1]);
            work_dir[strcspn(work_dir, "\n")] = 0;
        }
        if (strcmp(COM[i], "-b") == 0)
        {
            if (strlen(COM[i + 1]) > 128)
            {
                printf(L_BIN_PATH_TOOLONG);
                return 1;
            }
            if (access(COM[i + 1], F_OK) != 0)
            {
                printf(L_BIN_PATH_NOTFIND);
                return 1;
            }
            snprintf(bin_dir, sizeof(bin_dir), "%s", COM[i + 1]);
            bin_dir[strcspn(bin_dir, "\n")] = 0;
        }
        if (strcmp(COM[i], "-s") == 0)
        {
            if (strlen(COM[i + 1]) > 5)
            {
                printf(L_MODE_TOOLONG);
                return 1;
            }
            snprintf(mode, sizeof(mode), "%s", COM[i + 1]);
            mode[strcspn(mode, "\n")] = 0;
        }
    }
    if (strcmp(work_dir, "") == 0)
    {
        printf(L_ARG_CONFIGPATH_ERR);
        return 1;
    }
    if (strcmp(bin_dir, "") == 0)
    {
        printf(L_ARG_BINPATH_ERR);
        return 1;
    }
    if (strcmp(mode, "") == 0)
    {
        printf(L_ARG_MODE_ERR);
        return 1;
    }
    
    // Stop & Reset
    char command[strlen(bin_dir) + strlen(DATA_DIR) + 64];
    
    if (strcasecmp(mode, "STOP") == 0)
    {
        snprintf(command, sizeof(command), STOP_COMMAND, bin_dir, DATA_DIR);
        if (system(command) == 0)
        {
            printf(L_SS_OPEN_STOP_STORAGE_SUCCESSFUL);
            stop = 1;
        }
        else
        {
            printf(L_SS_OPEN_STOP_STORAGE_FAILED);
            return 1;
        }
    }
    else if (strcasecmp(mode, "RESET") == 0)
    {
        snprintf(command, sizeof(command), RESET_COMMAND, bin_dir, DATA_DIR);
        if (system(command) == 0)
        {
            printf(L_SS_OFF_STOP_STORAGE_SUCCESSFUL);
            stop = 0;
        }
        else
        {
            printf(L_SS_OFF_STOP_STORAGE_FAILED);
            return 1;
        }
    }
    else
    {
        printf(L_MODE_ERR, mode);
        return 1;
    }
    
    char set_prop[strlen(work_dir) + 128];
    if (stop == 1)
    {
        snprintf(set_prop, sizeof(set_prop), SETPROP_STOP, work_dir);
    }
    else
    {
        snprintf(set_prop, sizeof(set_prop), SETPROP_RESET, work_dir);
    }
    if (system(set_prop) != 0)
    {
        printf(L_W_SETPROP_ERR);
    }
    
    return 0;
}
