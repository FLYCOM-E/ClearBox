/*
                    GNU GENERAL PUBLIC
                        Version 3

     此 Code 来自 ClearBox 模块，用于删除垃圾缓存及空文件夹
*/

#include "INCLUDE/BashCore.h"

#define WHITELIST "ClearWhitelist.prop"     // 白名单文件名
#define CARD_HOME "/mnt/media_rw"           // 外置 SD 卡根目录
#define STORAGES_DIR "/data/media/0"        // 储存目录

static int storage_cache_clear(char * data_path, char * work_dir);
static int storage_clear(char * storage_dir, int home);

int storage_clean(char * work_dir)
{
    int clean_count = 0;
    char settings_file[strlen(work_dir) + strlen(SETTINGS_FILE) + 2];
    snprintf(settings_file, sizeof(settings_file), "%s/%s", work_dir, SETTINGS_FILE);
    
    // 设置查找对应值
    int clear_disk = get_settings_prop(settings_file, "clearbox_clear_disk");
    
    // 处理内部储存
    storage_cache_clear(STORAGES_DIR, work_dir);
    clean_count = storage_clear(STORAGES_DIR, 1);
    
    if (clean_count == -1)
    {
        fprintf(stderr, L_SC_CLEAR_DIRTY_ERR);
    }
    else
    {
        fprintf(stderr, L_SC_SUCCESSFUL_STORAGE, clean_count);
    }
    fflush(stdout);
    
    if (clear_disk != 1) // 1 允许清理外部储存
    {
        return 0;
    }
    
    struct dirent * entry;
    DIR * sdcard_id_dp = opendir(CARD_HOME);
    if (sdcard_id_dp == NULL)
    {
        fprintf(stderr, L_OPEN_PATH_FAILED, CARD_HOME, strerror(errno));
        return 0;
    }
    while ((entry = readdir(sdcard_id_dp)))
    {
        if (strcmp(entry -> d_name, ".") == 0 ||
           strcmp(entry -> d_name, "..") == 0)
        {
            continue;
        }
        
        char sdcard_dir[sizeof(CARD_HOME) + strlen(entry -> d_name) + 2];
        snprintf(sdcard_dir, sizeof(sdcard_dir), "%s/%s", CARD_HOME, entry -> d_name);
        
        // 调用函数（外部储存
        storage_cache_clear(sdcard_dir, work_dir);
        clean_count = storage_clear(sdcard_dir, 1);
        
        if (clean_count == -1)
        {
            fprintf(stderr, L_SC_CLEAR_DIRTY_ERR);
        }
        else
        {
            fprintf(stderr, L_SC_SUCCESSFUL_SD, entry -> d_name, clean_count);
            
        }
        fflush(stdout);
    }
    closedir(sdcard_id_dp);
    
    return 0;
}

/* 
内/外部储存App缓存清理函数
接收：
    char * data_path 储存根目录
    char * work_dir 配置目录
返回：
    int 成功返回0，失败返回1
*/
static int storage_cache_clear(char * data_path, char * work_dir)
{
    if (access(data_path, F_OK) != 0)
    {
        fprintf(stderr, L_OPEN_PATH_FAILED, data_path, strerror(errno));
        return 1;
    }
    
    // 拼接路径/白名单文件
    int clean_count = 0;
    char whitelist_file[strlen(work_dir) + strlen(WHITELIST) + 2],
         app_data_path[strlen(data_path) + 32];
    snprintf(app_data_path, sizeof(app_data_path), "%s/Android/data", data_path);
    snprintf(whitelist_file, sizeof(whitelist_file), "%s/%s", work_dir, WHITELIST);
    
    // 递归遍历目录
    struct dirent * entry;
    DIR * app_data_dir_dp = opendir(app_data_path);
    if (app_data_dir_dp == NULL)
    {
        fprintf(stderr, L_OPEN_PATH_FAILED, app_data_path, strerror(errno));
        return 1;
    }
    while ((entry = readdir(app_data_dir_dp)))
    {
        if (strcmp(entry -> d_name, ".") == 0 ||
           strcmp(entry -> d_name, "..") == 0)
        {
            continue;
        }
        
        // 完整缓存路径
        char app_path[strlen(entry -> d_name) + strlen(app_data_path) + 32];
        snprintf(app_path, sizeof(app_path), "%s/%s/cache", app_data_path, entry -> d_name);
        if (access(app_path, F_OK) != 0)
        {
            continue;
        }
        
        // 白名单检查
        if (whitelist_check(whitelist_file, entry -> d_name) == 1)
        {
            continue;
        }
        
        // Clear
        if (s_remove(app_path, 0) != -1)
        {
            printf(L_SC_CLEAR, entry -> d_name);
            clean_count++;
        }
        else
        {
            fprintf(stderr, L_SC_CLEAR_ERR, entry -> d_name);
        }
        fflush(stdout);
    }
    closedir(app_data_dir_dp);
    
    fprintf(stderr, L_SC_CLEAR_CACHE_DONE, clean_count);
    return 0;
}

/*
递归清理函数
接收：
    char * dir 储存根目录
    int home 如果外置储存根目录为空是否删除该根目录，默认使用1（跳过删除）
返回：
    int 成功返回清理空文件夹/文件数量，失败返回-1
*/
static int storage_clear(char * dir, int home)
{
    if (access(dir, F_OK) != 0)
    {
        return -1;
    }
    
    // 打开目录
    int count = 0;
    int count_all = 0;
    struct stat file_stat;
    struct dirent * entry;
    
    DIR * dir_dp = opendir(dir);
    if (dir_dp == NULL)
    {
        fprintf(stderr, L_OPEN_PATH_FAILED, dir, strerror(errno));
        return -1;
    }
    
    // 遍历目录
    while ((entry = readdir(dir_dp)))
    {
        if (strcmp(entry -> d_name, ".") == 0 ||
           strcmp(entry -> d_name, "..") == 0)
        {
            continue;
        }
        
        count_all++;
        char path[strlen(dir) + strlen(entry -> d_name) + 32];
        snprintf(path, sizeof(path), "%s/%s", dir, entry -> d_name);
        
        if (lstat(path, &file_stat) == -1)
        {
            continue;
        }
        
        if (S_ISLNK(file_stat.st_mode))
        {
            continue;
        }
        if (S_ISDIR(file_stat.st_mode))
        {
            // 检查是否为媒体缓存目录，是则直接删除
            if (strcmp(entry -> d_name, ".thumbnails") == 0)
            {
                if (s_remove(path, 1) != -1)
                {
                    count++;
                    count_all--;
                    continue;
                }
            }
            count += storage_clear(path, 0);
            if (access(path, F_OK) != 0) count_all--;
        }
        else
        {
            // 文件大小小于1，空文件
            if (file_stat.st_size < 1)
            {
                if (remove(path) == 0)
                {
                    count++;
                    count_all--;
                }
                continue;
            }
            char * str = strrchr(entry -> d_name, '.');
            if (str)
            {
                // Log文件
                if (strcmp(str + 1, "log") == 0)
                {
                    if (remove(path) == 0)
                    {
                        count++;
                        count_all--;
                    }
                }
            }
        }
        
    }
    closedir(dir_dp);
    
    // 检查是否应该删除目录本身（递归时删除空目录）
    if (home == 0)
    {
        if (count_all == 0)
        {
            if (remove(dir) == 0)
            {
                count++;
            }
        }
    }
    
    return count;
}
