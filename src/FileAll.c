// 此Code来自ClearBox模块，用于内部储存指定格式文件清理
#include "BashCore.h"

#define MAX_ARGS_SIZE 32
#define CONFIG_MAX_ARGS 512
#define F_DIR_NAME "Documents"
#define SETTINGS_FILE_NAME "settings.prop" //Max Size 14
#define CONFIG_DIR_NAME "文件格式配置"
#define GET_SDCARD_ID "ls /storage | grep .*- 2>/dev/null"
#define STORAGES_DIR "/storage/%s" //Max Size 100

static int ClearService(char * work_dir, char * storage_dir);
static int FindFile(char * storage, char * file_dir, char args[][MAX_ARGS_SIZE], int count);

int main(int COMI, char * COM[])
{
    if (getuid() != 0)
    {
        printf(L_NOT_USE_ROOT);
        return 1;
    }
    if (COMI < 3)
    {
        printf(L_ARGS_FAILED);
        return 1;
    }
    
    char work_dir[128] = "";
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
    }
    if (strcmp(work_dir, "") == 0)
    {
        printf(L_ARG_CONFIGPATH_ERR);
        return 1;
    }
    
    int Fileall_Disk = 0;
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
            if (strcmp(settings_file_line, "Fileall_Disk=1") == 0)
            {
                Fileall_Disk = 1;
            }
        }
        fclose(settings_file_fp);
    }
    
    if (ClearService(work_dir, data_dir) == 0)
    {
        printf(L_FA_SUCCESSFUL_STORAGE);
    }
    else
    {
        printf(L_FA_FAILED_STORAGE);
    }
    
    if (Fileall_Disk == 1)
    {
        if (access(sdcard_dir, F_OK) == 0)
        {
            if (ClearService(work_dir, sdcard_dir) == 0)
            {
                printf(L_FA_SUCCESSFUL_SD);
            }
            else
            {
                printf(L_FA_FAILED_SD);
            }
        }
    }
    
    return 0;
}

/*
文件配置读取/清理函数
接收：
    char * work_dir 配置目录路径
    char * storage_dir 储存根目录
返回：
    int 成功返回0，失败返回1
*/
static int ClearService(char * work_dir, char * storage_dir)
{   
    if (access(work_dir, F_OK) != 0 || access(storage_dir, F_OK) != 0)
    {
        return 1;
    }
    
    char config_dir[strlen(work_dir) + strlen(CONFIG_DIR_NAME) + 2], f_dir[strlen(storage_dir) + strlen(F_DIR_NAME) + 8];
    
    snprintf(config_dir, sizeof(config_dir), "%s/%s", work_dir, CONFIG_DIR_NAME);
    snprintf(f_dir, sizeof(f_dir), "%s/%s", storage_dir, F_DIR_NAME);
    mkdir(f_dir, 0775);
    
    struct dirent * entry;
    DIR * config_dir_dp = opendir(config_dir);
    if (config_dir_dp == NULL)
    {
        printf(L_OPEN_PATH_FAILED, config_dir);
        return 1;
    }
    
    while ((entry = readdir(config_dir_dp)))
    {
        if (strcmp(entry -> d_name, ".") == 0 ||
           strcmp(entry -> d_name, "..") == 0)
        {
            continue;
        }
        
        char config_file[strlen(config_dir) + strlen(entry -> d_name) + 2],
             config_file_name[strlen(entry -> d_name) + 2];
        
        snprintf(config_file_name, sizeof(config_file_name), "%s", entry -> d_name);
        snprintf(config_file, sizeof(config_file), "%s/%s", config_dir, entry -> d_name);
        
        char * config_file_name_p = strtok(config_file_name, ".");
        char file_dir[strlen(F_DIR_NAME) + strlen(storage_dir) + strlen(config_file_name_p) + 8];
        snprintf(file_dir, sizeof(file_dir), "%s/%s/%s", storage_dir, F_DIR_NAME, config_file_name_p);
        
        mkdir(file_dir, 0775);
        
        if (access(file_dir, F_OK) != 0)
        {
            return 1;
        }
        
        int all_count = 0;
        FILE * config_file_fp = fopen(config_file, "r");
        if (config_file_fp == NULL)
        {
            printf(L_OPEN_FILE_FAILED, config_file);
            continue;
        }
        
        printf(L_FA_START, config_file_name_p);
        fflush(stdout);
        
        int count = 0;
        char file_args[CONFIG_MAX_ARGS][MAX_ARGS_SIZE] = {0};
        while (fscanf(config_file_fp, "%s", file_args[count]) == 1)
        {
            count++;
        }
        
        all_count += FindFile(storage_dir, file_dir, file_args, count);
        fclose(config_file_fp);
        printf(L_FA_END, all_count, config_file_name_p);
        fflush(stdout);
    }
    closedir(config_dir_dp);
    return 0;
}

/*
文件递归查找归类函数
接收：
    char * storage 储存根目录
    char * file_dir 归类目录
    char * str 查找文件后缀
返回：
    int 成功返回归类文件数量，失败返回-1
*/
static int FindFile(char * storage, char * file_dir, char args[][MAX_ARGS_SIZE], int count)
{
    if (access(storage, F_OK) != 0 || access(file_dir, F_OK) != 0)
    {
        return 0;
    }
    
    int file_count = 0;
    struct dirent * entry;
    struct stat file_stat;
    DIR * storage_dp = opendir(storage);
    if (storage_dp == NULL)
    {
        return 0;
    }
    
    while ((entry = readdir(storage_dp)))
    {
        if (strcmp(entry -> d_name, ".") == 0 ||
           strcmp(entry -> d_name, "..") == 0)
        {
            continue;
        }
        
        char file_name[strlen(entry -> d_name) + 2];
        char path[strlen(storage) + strlen(entry -> d_name) + 64];
        char end_path[strlen(file_dir) + strlen(entry -> d_name) + 128];
        
        snprintf(file_name, sizeof(file_name), "%s", entry -> d_name);
        snprintf(path, sizeof(path), "%s/%s", storage, entry -> d_name);
        snprintf(end_path, sizeof(end_path), "%s/%s", file_dir, entry -> d_name);
        
        if (strcmp(path, file_dir) == 0 ||
           stat(path, &file_stat) == -1)
        {
            continue;
        }
        
        if (S_ISDIR(file_stat.st_mode))
        {
            file_count += FindFile(path, file_dir, args, count);
        }
        else
        {
            char * str_p = strrchr(file_name, '.');
            if (str_p != NULL)
            {
                for (int i = 0; i < count; i++)
                {
                    if (strcasecmp(args[i], str_p + 1) == 0)
                    {
                        int name_i = 1;
                        while (access(end_path, F_OK) == 0)
                        {
                            snprintf(end_path, sizeof(end_path), "%s/%s.%d.bak", file_dir, entry -> d_name, name_i);
                            name_i++;
                        }
                        
                        if (rename(path, end_path) == 0)
                        {
                            file_count++;
                        }
                        else
                        {
                            printf(L_MOVE_ERROR, path);
                        }
                        break;
                    }
                }
            }
        }
    }
    closedir(storage_dp);
    return file_count;
}
