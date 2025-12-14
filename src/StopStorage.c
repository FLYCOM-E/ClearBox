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
        printf(" » 请授予root权限！\n");
        return 1;
    }
    if (COMI < 6)
    {
        printf(" » 参数不足！\n");
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
                printf(" » 配置路径过长！\n");
                return 1;
            }
            if (access(COM[i + 1], F_OK) != 0)
            {
                printf(" » 配置路径不可访问！\n");
                return 1;
            }
            snprintf(work_dir, sizeof(work_dir), "%s", COM[i + 1]);
            work_dir[strcspn(work_dir, "\n")] = 0;
        }
        if (strcmp(COM[i], "-b") == 0)
        {
            if (strlen(COM[i + 1]) > 128)
            {
                printf(" » Bin路径过长！\n");
                return 1;
            }
            if (access(COM[i + 1], F_OK) != 0)
            {
                printf(" » Bin路径不可访问！\n");
                return 1;
            }
            snprintf(bin_dir, sizeof(bin_dir), "%s", COM[i + 1]);
            bin_dir[strcspn(bin_dir, "\n")] = 0;
        }
        if (strcmp(COM[i], "-s") == 0)
        {
            if (strlen(COM[i + 1]) > 5)
            {
                printf(" » 模式错误！参数过长！\n");
                return 1;
            }
            snprintf(mode, sizeof(mode), "%s", COM[i + 1]);
            mode[strcspn(mode, "\n")] = 0;
        }
    }
    if (strcmp(work_dir, "") == 0)
    {
        printf(" » 未传入配置目录！\n");
        return 1;
    }
    if (strcmp(bin_dir, "") == 0)
    {
        printf(" » 未传入Bin目录！\n");
        return 1;
    }
    
    // Stop & Reset
    char command[strlen(bin_dir) + strlen(DATA_DIR) + 64];
    
    if (strcasecmp(mode, "STOP") == 0)
    {
        snprintf(command, sizeof(command), STOP_COMMAND, bin_dir, DATA_DIR);
        if (system(command) == 0)
        {
            printf(" » 已开启内部储存固定！\n");
            stop = 1;
        }
        else
        {
            printf(" » 开启内部储存固定失败！\n");
            return 1;
        }
    }
    else if (strcasecmp(mode, "RESET") == 0)
    {
        snprintf(command, sizeof(command), RESET_COMMAND, bin_dir, DATA_DIR);
        if (system(command) == 0)
        {
            printf(" » 已关闭内部储存固定！\n");
            stop = 0;
        }
        else
        {
            printf(" » 关闭内部储存固定失败！\n");
            return 1;
        }
    }
    else
    {
        printf(" » 未知模式！[%s]\n", mode);
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
        printf(" » 警告：PROP设置失败！\n");
    }
    
    return 0;
}
