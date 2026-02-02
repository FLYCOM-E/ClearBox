// 此Code来自ClearBox模块，用于阻止安装功能
#include "INCLUDE/BashCore.h"

#define DATA_DIR "/data/app"
#define STOP_INSTALL "%s/busybox chattr +i %s 2>/dev/null" //Max Size 60
#define RESET_INSTALL "%s/busybox chattr -i %s 2>/dev/null" //Max Size 60
#define SETPROP_STOP "sed -i 's/stopinstall=0/stopinstall=1/g' %s/settings.prop" //Max Size 126
#define SETPROP_RESET "sed -i 's/stopinstall=1/stopinstall=0/g' %s/settings.prop" //Max Size 126

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
