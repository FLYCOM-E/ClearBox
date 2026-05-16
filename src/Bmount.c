/*
                    GNU GENERAL PUBLIC
                        Version 3

     此 Code 来自 ClearBox 模块，用于储存重定向规则解析与挂载
*/

#include "INCLUDE/BashCore.h"

#define CONFIG_DIR_NAME "BindConfigs"         // 配置目录名
#define DATA_DIR "/data/media/0"                // 内部储存根目录

static int bind_mount(char * config_file);

int bmount(char * work_dir)
{
    if (access(work_dir, F_OK) != 0)
    {
        fprintf(stderr, L_OPEN_PATH_FAILED, work_dir, strerror(errno));
        return 1;
    }
    
    char config_dir[strlen(work_dir) + sizeof(CONFIG_DIR_NAME) + 2];
    snprintf(config_dir, sizeof(config_dir), "%s/%s", work_dir, CONFIG_DIR_NAME);
    
    struct dirent * entry;
    DIR * config_dir_dp = opendir(config_dir);
    if (config_dir_dp == NULL)
    {
        fprintf(stderr, L_OPEN_PATH_FAILED, config_dir, strerror(errno));
        return 1;
    }
    
    while ((entry = readdir(config_dir_dp)))
    {
        if (strcmp(entry -> d_name, ".") == 0 ||
           strcmp(entry -> d_name, "..") == 0)
        {
            continue;
        }
        
        char config_file[strlen(config_dir) + strlen(entry -> d_name) + 2];
        snprintf(config_file, sizeof(config_file), "%s/%s", config_dir, entry -> d_name);
        
        bind_mount(config_file);
    }
    closedir(config_dir_dp);
    
    return 0;
}

/*
配置解析与挂载
接收：
    char * config_file 配置文件
返回：
    成功返回 0，失败返回 1
*/
static int bind_mount(char * config_file)
{
    FILE * config_file_fp = fopen(config_file, "r");
    if (config_file_fp == NULL)
    {
        fprintf(stderr, L_OPEN_FILE_FAILED, config_file, strerror(errno));
        return 1;
    }
    
    char line[PATH_MAX] = "";
    while (fgets(line, sizeof(line), config_file_fp))
    {
        if (strlen(line) < 1 ||
           line[0] == '#')
        {
            continue;
        }
        
        /*
        格式：
        相对（内部储存）目录路径
        DIR-1 DIR-2
        绑定 DIR-1 至 DIR-2
        */
        
        char * strtok_p = NULL,
             * root_dir_p = NULL,
             * bind_dir_p = NULL;
        root_dir_p = strtok_r(line, " ", &strtok_p);
        bind_dir_p = strtok_r(NULL, " ", &strtok_p);
        
        if (root_dir_p && bind_dir_p)
        {
            char root_dir[sizeof(DATA_DIR) + strlen(root_dir_p) + 2],
                 bind_dir[sizeof(DATA_DIR) + strlen(bind_dir_p) + 2];
            snprintf(root_dir, sizeof(root_dir), "%s/%s", DATA_DIR, root_dir_p);
            snprintf(bind_dir, sizeof(bind_dir), "%s/%s", DATA_DIR, bind_dir_p);
            
            if (access(root_dir, F_OK) != 0 ||
               access(bind_dir, F_OK) != 0)
            {
                continue;
            }
            
            if (mount(root_dir, bind_dir, NULL, MS_BIND, NULL) == -1)
            {
                fprintf(stderr, L_MOUNT_ERROR, root_dir, strerror(errno));
            }
            else
            {
                printf(L_BM_MOUNT_SUCCESS, root_dir, bind_dir);
            }
        }
    }
    fclose(config_file_fp);
    
    return 0;
}
