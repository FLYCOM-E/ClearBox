// 此脚本来自ClearBox模块，用于阻止安装功能
#include "BashCore.h"

#define DATA_DIR "/data/app"
#define MICRO_DIR "/mnt/expand/%s/app" //Max Size 100
#define GET_SD_ID "ls /mnt/expand 2>/dev/null"
#define STOP_INSTALL "%s/busybox chattr +i %s 2>/dev/null" //Max Size 60
#define RESET_INSTALL "%s/busybox chattr -i %s 2>/dev/null" //Max Size 60
#define SETPROP_STOP "sed -i 's/stopinstall=0/stopinstall=1/g' %s/settings.prop" //Max Size 126
#define SETPROP_RESET "sed -i 's/stopinstall=1/stopinstall=0/g' %s/settings.prop" //Max Size 126

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
    
    // Get SD Card ID
    char micro_dir[256] = "", card_id[128] = "";
    FILE * card_id_fp = popen(GET_SD_ID, "r");
    if (card_id_fp)
    {
        fgets(card_id, sizeof(card_id), card_id_fp);
        card_id[strcspn(card_id, "\n")] = 0;
        snprintf(micro_dir, sizeof(micro_dir), MICRO_DIR, card_id);
        pclose(card_id_fp);
    }
    
    // Stop & Reset
    char command_data[strlen(bin_dir) + strlen(DATA_DIR) + 64];
    char command_micro[strlen(bin_dir) + strlen(micro_dir) + 64];
    
    if (strcasecmp(mode, "STOP") == 0)
    {
        snprintf(command_data, sizeof(command_data), STOP_INSTALL, bin_dir, DATA_DIR);
        if (system(command_data) == 0)
        {
            printf(" » 内部储存已阻止更新！\n");
            stop = 1;
        }
        else
        {
            printf(" » 内部储存阻止更新失败！\n");
        }
        if (access(micro_dir, F_OK) == 0)
        {
            snprintf(command_micro, sizeof(command_micro), STOP_INSTALL, bin_dir, micro_dir);
            if (system(command_micro) == 0)
            {
                printf(" » 外部储存已阻止更新！\n");
            }
            else
            {
                printf(" » 外部储存阻止更新失败！\n");
            }
        }
    }
    else if (strcasecmp(mode, "RESET") == 0)
    {
        snprintf(command_data, sizeof(command_data), RESET_INSTALL, bin_dir, DATA_DIR);
        if (system(command_data) == 0)
        {
            printf(" » 内部储存已关闭阻止更新！\n");
            stop = 0;
        }
        else
        {
            printf(" » 内部储存关闭阻止更新失败！\n");
        }
        if (access(micro_dir, F_OK) == 0)
        {
            snprintf(command_micro, sizeof(command_micro), RESET_INSTALL, bin_dir, micro_dir);
            if (system(command_micro) == 0)
            {
                printf(" » 外部储存已关闭阻止更新！\n");
            }
            else
            {
                printf(" » 外部储存关闭阻止更新失败！\n");
            }
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
