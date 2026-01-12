// 此Code来自ClearBox模块，用于删除垃圾缓存及空文件夹
#include "INCLUDE/BashCore.h"

#define SETTINGS_FILE_NAME "settings.prop" //Max Size 14
#define WHITELIST "%s/ClearWhitelist.prop"
#define GET_SDCARD_ID "ls /mnt/media_rw | grep .*- 2>/dev/null"
#define STORAGES_DIR "/data/media/0" //Max Size 100
#define SDCARD_DIR "/mnt/media_rw/%s" //Max Size 100

static int DeleteAppCache(char * data_path, char * work_dir);
static int StorageClean(char * storage_dir);

int main(int argc, char * argv[])
{
    if (getuid() != 0)
    {
        fprintf(stderr, L_NOT_USE_ROOT);
        return 1;
    }
    
    argc--;
    argv++;
    if (argc < 1)
    {
        fprintf(stderr, L_ARGS_FAILED);
        return 1;
    }
    
    char * work_dir = NULL;
    
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
    
    int cleardisk = 0; // 是否清理外部储存
    char sdcard_id[16] = "", // 外置SD名称
         sdcard_dir[128] = "", // 外置SD完整路径
         data_dir[128] = "", // 内部储存完整路径
         settings_file[strlen(work_dir) + 16], // 设置信息储存文件
         settings_file_line[64] = "";
    
    // 获取外部储存名称（多储存暂不支持，后续...吧
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
    
    // 拼接路径
    snprintf(settings_file, sizeof(settings_file), "%s/%s", work_dir, SETTINGS_FILE_NAME);
    snprintf(data_dir, sizeof(data_dir), STORAGES_DIR);
    snprintf(sdcard_dir, sizeof(sdcard_dir), SDCARD_DIR, sdcard_id);
    
    // 打开设置信息文件并查找对应值
    FILE * settings_file_fp = fopen(settings_file, "r");
    if (settings_file_fp)
    {
        while (fgets(settings_file_line, sizeof(settings_file_line), settings_file_fp))
        {
            settings_file_line[strcspn(settings_file_line, "\n")] = 0;
            if (strcmp(settings_file_line, "cleardisk=1") == 0)
            {
                cleardisk = 1;
            }
        }
        fclose(settings_file_fp);
    }
    
    // 处理内部储存
    DeleteAppCache(data_dir, work_dir);
    int clean_count = StorageClean(data_dir);
    if (clean_count == -1)
    {
        fprintf(stderr, L_SC_CLEAR_DIRTY_ERR);
    }
    else
    {
        printf(L_SC_CLEAR_DIRTY, clean_count);
    }
    printf(L_SC_SUCCESSFUL_STORAGE);
    fflush(stdout);
    
    if (cleardisk == 1) // 允许清理外部储存
    {
        if (access(sdcard_dir, F_OK) == 0) // 外部储存存在则处理
        {
            DeleteAppCache(sdcard_dir, work_dir);
            clean_count = StorageClean(sdcard_dir);
            if (clean_count == -1)
            {
                fprintf(stderr, L_SC_CLEAR_DIRTY_ERR);
            }
            else
            {
                printf(L_SC_CLEAR_DIRTY, clean_count);
                
            }
            printf(L_SC_SUCCESSFUL_SD, sdcard_id);
            fflush(stdout);
        }
    }
    
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
static int DeleteAppCache(char * data_path, char * work_dir)
{
    if (access(data_path, F_OK) != 0)
    {
        fprintf(stderr, L_OPEN_PATH_FAILED, data_path);
        return 1;
    }
    
    // 拼接/定义路径
    int clean_count = 0;
    char whitelist_file[strlen(work_dir) + strlen(WHITELIST) + 2],
         app_data_path[strlen(data_path) + 32];
    snprintf(app_data_path, sizeof(app_data_path), "%s/Android/data", data_path);
    snprintf(whitelist_file, sizeof(whitelist_file), WHITELIST, work_dir);
    
    // 打开目录
    struct dirent * entry;
    DIR * app_data_dir_dp = opendir(app_data_path);
    if (app_data_dir_dp == NULL)
    {
        fprintf(stderr, L_OPEN_PATH_FAILED, app_data_path);
        return 1;
    }
    
    // 遍历目录
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
        if (whiteListCheck(whitelist_file, entry -> d_name) == 1)
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
    
    printf(L_SC_CLEAR_CACHE_DONE, clean_count);
    return 0;
}

/*
递归清理函数
接收：
    char * storage_dir 储存根目录
返回：
    int 成功返回清理空文件夹/文件数量，失败返回-1
*/
static int StorageClean(char * dir)
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
            if (strcmp(entry -> d_name, ".thumbnails") == 0)
            {
                if (s_remove(path, 1) != -1)
                {
                    count++;
                    count_all--;
                    continue;
                }
            }
            count += StorageClean(path);
            if (access(path, F_OK) != 0) count_all--;
        }
        else
        {
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
    
    if (count_all == 0)
    {
        if (remove(dir) == 0)
        {
            count++;
        }
    }
    
    return count;
}
