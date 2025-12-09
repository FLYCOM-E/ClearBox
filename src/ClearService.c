// 此Core来自ClearBox模块，用于内部储存指定格式文件清理
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define SETTINGS_FILE_NAME "settings.prop" //Max Size 14
#define CONFIG_DIR_NAME "文件格式配置" //Max Size 30
#define GET_SDCARD_ID "ls /storage | grep .*- 2>/dev/null"
#define STORAGES_DIR "/storage/%s" //Max Size 100

static int ClearService(char * name, char * work_dir, char * storage_dir);
static int FindFile(char * storage, char * str);

int main(int COMI, char * COM[])
{
    if (getuid() != 0)
    {
        printf(" » 请授予root权限！\n");
        return 1;
    }
    if (COMI < 5)
    {
        printf(" » 参数不足！\n");
        return 1;
    }
    
    char work_dir[128] = "", name[64] = "";
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
                printf(" » 配置路径不存在/无法访问！\n");
                return 1;
            }
            snprintf(work_dir, sizeof(work_dir), "%s", COM[i + 1]);
            work_dir[strcspn(work_dir, "\n")] = 0;
        }
        if (strcmp(COM[i], "-m") == 0)
        {
            if (strlen(COM[i + 1]) > 60)
            {
                printf(" » 传入清理项名称过长！\n");
                return 1;
            }
            snprintf(name, sizeof(name), "%s", COM[i + 1]);
            name[strcspn(name, "\n")] = 0;
        }
    }
    if (strcmp(work_dir, "") == 0)
    {
        printf(" » 未传入配置目录！\n");
        return 1;
    }
    if (strcmp(name, "") == 0)
    {
        printf(" » 未传入清理项！\n");
        return 1;
    }
    
    int FileClear_Disk = 0;
    char data_dir[128] = "",
         sdcard_dir[128] = "",
         sdcard_id[16] = "",
         settings_file[strlen(work_dir) + 16],
         settings_file_line[64] = "";
    
    FILE * sdcard_id_fp = popen(GET_SDCARD_ID, "r");
    if (sdcard_id_fp)
    {
        fgets(sdcard_id, sizeof(sdcard_id), sdcard_id_fp);
        pclose(sdcard_id_fp);
        sdcard_id[strcspn(sdcard_id, "\n")] = 0;
        if (strcmp(sdcard_id, "") == 0)
        {
            strcpy(sdcard_id, "(null)");
        }
    }
    
    snprintf(settings_file, sizeof(settings_file), "%s/%s", work_dir, SETTINGS_FILE_NAME);
    snprintf(data_dir, sizeof(data_dir), STORAGES_DIR, "emulated/0");
    snprintf(sdcard_dir, sizeof(sdcard_dir), STORAGES_DIR, sdcard_id);
    
    // 打开设置信息文件并查找对应值
    FILE * settings_file_fp = fopen(settings_file, "r");
    if (settings_file_fp)
    {
        while (fgets(settings_file_line, sizeof(settings_file_line), settings_file_fp))
        {
            settings_file_line[strcspn(settings_file_line, "\n")] = 0;
            if (strcmp(settings_file_line, "FileClear_Disk=1") == 0)
            {
                FileClear_Disk = 1;
            }
        }
        fclose(settings_file_fp);
    }
    
    if (ClearService(name, work_dir, data_dir) == 0)
    {
        printf(" » 清理内部储存 %s 成功！\n", name);
    }
    else
    {
        printf(" » 清理内部储存 %s 失败！\n", name);
    }
    
    if (FileClear_Disk == 1)
    {
        if (access(sdcard_dir, F_OK) == 0)
        {
            if (ClearService(name, work_dir, sdcard_dir) == 0)
            {
                printf(" » 清理外部储存 %s 成功！\n", name);
            }
            else
            {
                printf(" » 清理外部储存 %s 失败！\n", name);
            }
        }
    }
    
    return 0;
}

/*
文件配置读取函数
接收：
    char * name 配置文件名称（会直接作为配置名称使用，不带后缀）
    char * work_dir 配置文件目录，注意只是根配置目录不是完整配置路径，这个由宏决定
    char * storage_dir 储存根目录
返回：
    1：错误
    0：成功
*/
static int ClearService(char * name, char * work_dir, char * storage_dir)
{   
    if (access(work_dir, F_OK) != 0 || access(storage_dir, F_OK) != 0)
    {
        return 1;
    }
    
    char config_file[strlen(work_dir) + strlen(name) + 32];
    snprintf(config_file, sizeof(config_file), "%s/%s/%s.conf", work_dir, CONFIG_DIR_NAME, name);
    
    if (access(config_file, F_OK) != 0)
    {
        printf(" » %s 对应配置文件未找到！\n", name);
        return 1;
    }
    
    int clear_count = 0;
    char config_file_line[64] = "";
    FILE * config_file_fp = fopen(config_file, "r");
    if (config_file_fp == NULL)
    {
        printf(" » %s 配置文件打开失败！\n", config_file);
        return 1;
    }
    
    while (fscanf(config_file_fp, "%s", config_file_line) == 1)
    {
        if (strstr(config_file_line, "#"))
        {
            continue;
        }
        clear_count += FindFile(storage_dir, config_file_line);
    }
    fclose(config_file_fp);
    
    printf(" » 已清理 %d 个 %s\n", clear_count, name);
    return 0;
}

/*
递归查找删除函数
传入：
    char * storage 开始查找目录
    char * str 要删除的文件后缀
返回：
    成功返回已清理文件数，失败返回-1
*/
static int FindFile(char * storage, char * str)
{
    if (access(storage, F_OK) != 0)
    {
        return -1;
    }
    
    int file_count = 0;
    struct dirent * entry;
    struct stat file_stat;
    DIR * storage_dp = opendir(storage);
    if (storage_dp == NULL)
    {
        return -1;
    }
    
    while ((entry = readdir(storage_dp)))
    {
        if (strcmp(entry -> d_name, ".") == 0 ||
           strcmp(entry -> d_name, "..") == 0)
        {
            continue;
        }
        
        char file_name[strlen(entry -> d_name) + 2],
             path[strlen(storage) + strlen(entry -> d_name) + 32];
        snprintf(file_name, sizeof(file_name), "%s", entry -> d_name);
        snprintf(path, sizeof(path), "%s/%s", storage, entry -> d_name);
        
        if (stat(path, &file_stat) == -1)
        {
            continue;
        }
        
        if (S_ISDIR(file_stat.st_mode))
        {
            file_count += FindFile(path, str);
        }
        else
        {
            char * str_p = strrchr(file_name, '.');
            if (str_p != NULL && strcmp(str_p + 1, str) == 0)
            {
                remove(path);
                file_count++;
            }
        }
    }
    closedir(storage_dp);
    return file_count;
}
