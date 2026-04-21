/*
                    GNU GENERAL PUBLIC
                        Version 3

     此 Code 来自 ClearBox 模块，用于内部储存指定格式文件清理
*/

#include "INCLUDE/BashCore.h"

#define MAX_CONFIG_NAME 256             // 配置名称长度限制（不含.conf后缀)
#define MAX_ARGS_SIZE 32                // 后缀名称长度限制
#define CONFIG_MAX_ARGS 5000            // 单个文件格式配置最多允许的后缀数量
#define F_DIR_NAME "Documents"          // 归类目录名称（仅文件归类模式会用）
#define CONFIG_DIR_NAME "FileConfigs"   // 配置文件夹名称
#define CARD_HOME "/mnt/media_rw"      // 外置储存根目录
#define STORAGES_DIR "/data/media/0"   //Max Size 100

static int clear_service(char * work_dir, char * storage_dir, char * config_name);
static int find_file(char * storage, char * file_dir, char args[][MAX_ARGS_SIZE], int count);

int file_clear = 0;

int file_manager(char * work_dir, int mode, char * config_name)
{
    if (mode == 0)
    {
        // 文件清理模式需要传入配置名称
        file_clear = 1;
    }
    else if (mode == 1)
    {
        config_name = NULL;
    }
    
    int file_clear_disk = 0, // 是否清理拓展储存文件
        file_all_disk = 0;   // 是否归类拓展储存文件
    char settings_file[strlen(work_dir) + strlen(SETTINGS_FILE) + 2];
    snprintf(settings_file, sizeof(settings_file), "%s/%s", work_dir, SETTINGS_FILE);
    
    // 查找设置对应值
    file_all_disk = get_settings_prop(settings_file, "clearbox_file_all_disk");
    file_clear_disk = get_settings_prop(settings_file, "clearbox_file_clear_disk");
    
    // 调用函数（内部储存
    if (clear_service(work_dir, STORAGES_DIR, config_name) == 0)
    {
        if (file_clear == 1)
        {
            printf(L_FM_CR_SUCCESSFUL_STORAGE);
        }
        else
        {
            printf(L_FM_ALL_SUCCESSFUL_STORAGE);
        }
    }
    else
    {
        if (file_clear == 1)
        {
            fprintf(stderr, L_FM_CR_FAILED_STORAGE);
        }
        else
        {
            fprintf(stderr, L_FM_ALL_FAILED_STORAGE);
        }
    }
    fflush(stdout);
    
    // 根据模式判断是否处理拓展储存
    if (file_clear == 1)
    {
        if (file_clear_disk != 1)
        {
            return 0;
        }
    }
    else
    {
        if (file_all_disk != 1)
        {
            return 0;
        }
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
        if (access(sdcard_dir, F_OK) != 0)
        {
            continue;
        }
        
        // 调用函数（外部储存
        if (clear_service(work_dir, sdcard_dir, config_name) == 0)
        {
            if (file_clear == 1)
            {
                printf(L_FM_CR_SUCCESSFUL_SD);
            }
            else
            {
                printf(L_FM_ALL_SUCCESSFUL_SD);
            }
            fflush(stdout);
        }
        else
        {
            if (file_clear == 1)
            {
                fprintf(stderr, L_FM_CR_FAILED_SD);
            }
            else
            {
                fprintf(stderr, L_FM_ALL_FAILED_SD);
            }
        }
    }
    closedir(sdcard_id_dp);
    
    return 0;
}

/*
文件配置读取/清理函数
接收：
    char * work_dir 配置目录路径
    char * storage_dir 储存根目录
    char * config_name 配置名称（仅文件清理模式需要）
返回：
    int 成功返回 0，失败返回 1
*/
static int clear_service(char * work_dir, char * storage_dir, char * config_name)
{   
    if (access(work_dir, F_OK) != 0 || access(storage_dir, F_OK) != 0)
    {
        return 1;
    }
    
    char config_dir[strlen(work_dir) + strlen(CONFIG_DIR_NAME) + 2];
    snprintf(config_dir, sizeof(config_dir), "%s/%s", work_dir, CONFIG_DIR_NAME);
    
    // 文件清理模式
    if (file_clear == 1)
    {
        char config_file[strlen(config_dir) + strlen(config_name) + 16];
        snprintf(config_file, sizeof(config_file), "%s/%s.conf", config_dir, config_name);
        if (access(config_file, F_OK) != 0)
        {
            fprintf(stderr, L_CONFIG_NOTFOUND, config_name);
            return 1;
        }
        
        // 这里仍然定义归类目录，用于清理时跳过避免被清理
        char file_dir[strlen(F_DIR_NAME) + strlen(storage_dir) + strlen(config_name) + 8];
        snprintf(file_dir, sizeof(file_dir), "%s/%s/%s", storage_dir, F_DIR_NAME, config_name);
        
        FILE * config_file_fp = fopen(config_file, "r");
        if (config_file_fp == NULL)
        {
            fprintf(stderr, L_OPEN_FILE_FAILED, config_file, strerror(errno));
            return 1;
        }
        
        // 循环读取文件格式配置每个后缀并放进数组
        int count = 0;
        char file_args[CONFIG_MAX_ARGS][MAX_ARGS_SIZE] = {0};
        while (fscanf(config_file_fp, "%30s", file_args[count]) == 1)
        {
            count++;
        }
        fclose(config_file_fp);
        
        printf(L_FM_CR_START, config_name);
        fflush(stdout);
        
        // 文件清理
        int all_count = find_file(storage_dir, file_dir, file_args, count);
        
        fprintf(stderr, L_FM_CR_END, all_count, config_name);
    }
    else
    {
        // 定义/创建文件归类根目录
        char f_dir[strlen(storage_dir) + strlen(F_DIR_NAME) + 8];
        snprintf(f_dir, sizeof(f_dir), "%s/%s", storage_dir, F_DIR_NAME);
        if (access(f_dir, F_OK))
        {
            if (mkdir(f_dir, 0775) != 0)
            {
                fprintf(stderr, L_MKDIR_ERROR, f_dir, strerror(errno));
                return 1;
            }
        }
        
        // 遍历配置目录
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
            
            char config_file[strlen(config_dir) + strlen(entry -> d_name) + 2], // 完整路径文件
                 config_file_name[strlen(entry -> d_name) + 2];                 // 文件名
            snprintf(config_file_name, sizeof(config_file_name), "%s", entry -> d_name);
            snprintf(config_file, sizeof(config_file), "%s/%s", config_dir, entry -> d_name);
            
            // 提取文件名用于创建最终归类目录
            char * have_p = NULL;
            char * config_file_name_p = strtok_r(config_file_name, ".", &have_p);
            char file_dir[strlen(F_DIR_NAME) + strlen(storage_dir) + strlen(config_file_name_p) + 8];
            snprintf(file_dir, sizeof(file_dir), "%s/%s/%s", storage_dir, F_DIR_NAME, config_file_name_p);
            if (access(file_dir, F_OK) != 0)
            {
                if (mkdir(file_dir, 0775) != 0)
                {
                    fprintf(stderr, L_MKDIR_ERROR, f_dir, strerror(errno));
                    closedir(config_dir_dp);
                    return 1;
                }
            }
            
            FILE * config_file_fp = fopen(config_file, "r");
            if (config_file_fp == NULL)
            {
                fprintf(stderr, L_OPEN_FILE_FAILED, config_file, strerror(errno));
                continue;
            }
            
            // 循环读取文件格式配置每个后缀并放进数组
            int count = 0;
            char file_args[CONFIG_MAX_ARGS][MAX_ARGS_SIZE] = {0};
            while (fscanf(config_file_fp, "%30s", file_args[count]) == 1)
            {
                count++;
            }
            fclose(config_file_fp);
            
            printf(L_FM_ALL_START, config_file_name_p);
            fflush(stdout);
            
            int all_count = find_file(storage_dir, file_dir, file_args, count);
            
            fprintf(stderr, L_FM_ALL_END, all_count, config_file_name_p);
        }
        closedir(config_dir_dp);
    }
    return 0;
}

/*
文件递归查找处理函数（深度优先）
接收：
    char * storage 储存根目录
    char * file_dir 归类目录
    char args 文件后缀列表
    int count 文件后缀数量
返回：
    int 成功返回归类文件数量，失败返回 -1（不区分失败）
另：
    自动根据全局 file_clear 值 1 判断是否为文件清理模式
*/
static int find_file(char * storage, char * file_dir, char args[][MAX_ARGS_SIZE], int count)
{
    if (access(storage, F_OK) != 0 || access(file_dir, F_OK) != 0)
    {
        return -1;
    }
    
    // 打开传入目录开始递归遍历
    int file_count = 0;
    struct dirent * entry;
    struct stat file_stat;
    DIR * storage_dp = opendir(storage);
    if (storage_dp == NULL)
    {
        fprintf(stderr, L_OPEN_PATH_FAILED, storage, strerror(errno));
        return -1;
    }
    while ((entry = readdir(storage_dp)))
    {
        if (strcmp(entry -> d_name, ".") == 0 ||
           strcmp(entry -> d_name, "..") == 0)
        {
            continue;
        }
        
        char file_name[strlen(entry -> d_name) + 2];                              // 读取到的文件/目录名称
        char path[strlen(storage) + strlen(entry -> d_name) + 64];                // 读取到的文件/目录完整路径
        char end_path[strlen(file_dir) + strlen(entry -> d_name) + 128];          // 最终归类目录 (仅处于归类模式会用)
        snprintf(file_name, sizeof(file_name), "%s", entry -> d_name);
        snprintf(path, sizeof(path), "%s/%s", storage, entry -> d_name);
        snprintf(end_path, sizeof(end_path), "%s/%s", file_dir, entry -> d_name);
        
        if (strcmp(path, file_dir) == 0 ||
           lstat(path, &file_stat) == -1)
        {
            continue;
        }
        
        // 链接？跳过
        if (S_ISLNK(file_stat.st_mode))
        {
            continue;
        }
        if (S_ISDIR(file_stat.st_mode)) // 目录继续自调用递归
        {
            file_count += find_file(path, file_dir, args, count);
        }
        else // File
        {
            // 提取文件后缀进行匹配，这个会比较慢，视后缀数量
            char * str_p = strrchr(file_name, '.');
            if (str_p != NULL)
            {
                for (int i = 0; i < count; i++)
                {
                    if (strcasecmp(args[i], str_p + 1) == 0) // 这里匹配
                    {
                        if (file_clear == 1) // 清理模式直接清理并返回
                        {
                            if (remove(path) == 0)
                            {
                                file_count++;
                            }
                            else
                            {
                                fprintf(stderr, L_DELETE_ERROR, path, strerror(errno));
                            }
                            break;
                        }
                        
                        // 提取文件名称
                        char f_name[strlen(entry -> d_name) + 2];
                        snprintf(f_name, strlen(entry -> d_name) - strlen(str_p) + 1, "%s", entry -> d_name);
                        
                        // 如果文件名有重复则循环直到找到一个可用名称
                        int name_i = 1;
                        while (access(end_path, F_OK) == 0)
                        {
                            snprintf(end_path, sizeof(end_path), "%s/%s (%d).%s", file_dir, f_name, name_i, str_p + 1);
                            name_i++;
                        }
                        
                        // 移动文件
                        if (rename(path, end_path) == 0)
                        {
                            file_count++;
                        }
                        else
                        {
                            fprintf(stderr, L_MOVE_ERROR, path, strerror(errno));
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
