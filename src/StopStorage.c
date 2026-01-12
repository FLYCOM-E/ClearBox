// 此Code来自ClearBox模块，用于内部储存固定功能
#include "INCLUDE/BashCore.h"

#define DATA_DIR "/data/media/0"
#define STOP_COMMAND "%s/busybox chattr +i %s 2>/dev/null" //Max Size 62
#define RESET_COMMAND "%s/busybox chattr -i %s 2>/dev/null" //Max Size 62
#define SETPROP_STOP "sed -i 's/stopstorage=0/stopstorage=1/g' %s/settings.prop" //Max Size 126
#define SETPROP_RESET "sed -i 's/stopstorage=1/stopstorage=0/g' %s/settings.prop" //Max Size 126

int main(int argc, char * argv[])
{
    if (getuid() != 0)
    {
        fprintf(stderr, L_NOT_USE_ROOT);
        return 1;
    }
    
    argc--;
    argv++;
    if (argc < 5)
    {
        fprintf(stderr, L_ARGS_FAILED);
        return 1;
    }
    
    char * work_dir = NULL;
    char * bin_dir = NULL;
    char * mode = NULL;
    
    while (argc > 1)
    {
        if (strcmp(argv[0], "-w") == 0)
        {
            if (strlen(argv[1]) > MAX_WORK_DIR_LEN)
            {
                fprintf(stderr, L_CONFIG_PATH_TOOLONG);
                return 1;
            }
            if (access(argv[1], F_OK) != 0)
            {
                fprintf(stderr, L_CONFIG_PATH_NOTFOUND);
                return 1;
            }
            work_dir = argv[1];
            argc -= 2;
            argv += 2;
        }
        else if (strcmp(argv[0], "-b") == 0)
        {
            if (strlen(argv[1]) > MAX_BIN_DIR_LEN)
            {
                fprintf(stderr, L_BIN_PATH_TOOLONG);
                return 1;
            }
            if (access(argv[1], F_OK) != 0)
            {
                fprintf(stderr, L_BIN_PATH_NOTFOUND);
                return 1;
            }
            bin_dir = argv[1];
            argc -= 2;
            argv += 2;
        }
        else if (strcmp(argv[0], "-s") == 0)
        {
            if (strlen(argv[1]) > 5) // is 5
            {
                fprintf(stderr, L_MODE_TOOLONG);
                return 1;
            }
            mode = argv[1];
            argc -= 2;
            argv += 2;
        }
        else
        {
            fprintf(stderr, L_ARGS_FAILED_2);
            return 1;
        }
    }
    if (work_dir == NULL)
    {
        fprintf(stderr, L_ARG_CONFIGPATH_ERR);
        return 1;
    }
    if (bin_dir == NULL)
    {
        fprintf(stderr, L_ARG_BINPATH_ERR);
        return 1;
    }
    if (mode == NULL)
    {
        fprintf(stderr, L_ARG_MODE_ERR);
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
