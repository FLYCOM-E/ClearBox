// 此Core来自ClearBox模块，用于清空内部储存软件缓存
#include "BashCore.h"

#define DATA_DIR "/data/user" //Max Size 10
#define WHITELIST_FILE "ClearWhitelist.prop" //Max Size 30
#define SETTINGS_FILE "settings.prop" //Max Size 30
#define GET_APPLIST "pm list package -3 2>/dev/null"
#define GET_S_APPLIST "pm list package -s 2>/dev/null"
#define CLEAR_CACHE "rm -r %s/* 2>/dev/null" //Max Size 30

static int wipeCache(char * work_dir, char * whitelist_file, int ClearCacheSize);
static int whiteListCheck(char * whitelist_file, char * App);
static int GetPathSize(char * path);
static int ClearSystemCache();

int main(int COMI, char * COM[])
{
    if (getuid() != 0)
    {
        printf(" » 请授予root权限！\n");
        return 1;
    }
    else if (COMI < 4)
    {
        printf(" » 参数不足！\n");
        return 1;
    }
    
    // work_dir定义
    char work_dir[128] = "", mode[16] = "";
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
                printf(" » 配置路径不存在！\n");
                return 1;
            }
            snprintf(work_dir, sizeof(work_dir), "%s", COM[i + 1]);
            work_dir[strcspn(work_dir, "\n")] = 0;
        }
        if (strcmp(COM[i], "-m") == 0)
        {
            if (strlen(COM[i + 1]) > 14)
            {
                printf(" » 模式名称过长！\n");
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
    if (strcmp(mode, "") == 0)
    {
        printf(" » 未传入模式！\n");
        return 1;
    }
    
    // Case The Mode
    if (strcmp(mode, "AppCache_3") == 0)
    {
        // micro_dir定义
        char card_id[64] = "", micro_dir[128] = "";
        FILE * card_id_fp = popen("ls /mnt/expand/ | cut -f1 -d ' ' 2>/dev/null", "r");
        if (card_id_fp)
        {
            fgets(card_id, sizeof(card_id), card_id_fp);
            pclose(card_id_fp);
            card_id[strcspn(card_id, "\n")] = 0;
            snprintf(micro_dir, sizeof(micro_dir), "/mnt/expand/%s/user", card_id);
        }
        
        // whiteList定义
        char whitelist_file[strlen(work_dir) + 32];
        snprintf(whitelist_file, sizeof(whitelist_file), "%s/%s", work_dir, WHITELIST_FILE);
        
        /* 
        读取：
        ClearCacheSize（缓存清理限制大小）
        cleardisk（是否清理SD软件缓存）设置值
        */
        char * key_len_fp = NULL;
        int ClearCacheSize = 0, cleardisk = 0;
        char settings_file[strlen(work_dir) + 32], key_len[32] ="";
        snprintf(settings_file, sizeof(settings_file), "%s/%s", work_dir, SETTINGS_FILE);
        FILE * settings_file_fp = fopen(settings_file, "r");
        if (settings_file_fp)
        {
            while (fgets(key_len, sizeof(key_len), settings_file_fp))
            {
                key_len[strcspn(key_len, "\n")] = 0;
                if (strstr(key_len, "ClearCacheSize="))
                {
                    key_len_fp = strtok(key_len, "=");
                    key_len_fp = strtok(NULL, "=");
                    ClearCacheSize = atoi(key_len_fp);
                }
                if (strstr(key_len, "cleardisk="))
                {
                    key_len_fp = strtok(key_len, "=");
                    key_len_fp = strtok(NULL, "=");
                    cleardisk = atoi(key_len_fp);
                }
            }
            fclose(settings_file_fp);
        }
        
        //调用处理函数
        int clear_size = wipeCache(DATA_DIR, whitelist_file, ClearCacheSize);
        if (clear_size == -1)
        {
            printf(" » 内部储存软件缓存清理失败\n");
        }
        else
        {
            printf(" » 内部储存软件缓存清理完成\n » 共清理：%d兆\n", clear_size);
        }
        // cleardisk = 1：允许清理拓展SD缓存
        if (cleardisk == 1)
        {
            if (access(micro_dir, F_OK) == 0)
            {
                clear_size = wipeCache(micro_dir, whitelist_file, ClearCacheSize);
                if (clear_size == -1)
                {
                    printf(" » 外部储存软件缓存清理失败\n");
                }
                else
                {
                    printf(" » 外部储存软件缓存清理完成\n » 共清理：%d兆\n", clear_size);
                }
            }
        }
    }
    else if (strcmp(mode, "AppCache_S") == 0)
    {
        return ClearSystemCache();
    }
    else
    {
        printf(" » 未知模式！\n");
        return 1;
    }
    
    return 0;
}

/* 
此函数用于清理软件缓存，返回总清理大小
接收：
    char * work_dir
        软件数据目录，这里统一使用xxx/user，自动处理可能的多用户情况，兼容拓展SD
    int * ClearCacheSize
        缓存清理限制大小
返回：
    int 清理垃圾大小（单位：兆M），失败返回-1
*/
static int wipeCache(char * work_dir, char * whitelist_file, int ClearCacheSize)
{
    // 定义所需变量
    int cache_size = 0, clean_size = 0, count = 0, no_count = 0;
    char app_cache_dir[256] = "", package_list_line[MAX_PACKAGE] = "";
    
    // 打开user目录
    struct dirent * uid_dir;
    DIR * uid_dir_dp = opendir(work_dir);
    if (uid_dir_dp == NULL)
    {
        printf(" » 目录打开失败！\n");
        return -1;
    }
    
    // while遍历user目录（处理多用户
    while ((uid_dir = readdir(uid_dir_dp)))
    {
        if (strcmp(uid_dir -> d_name, ".") == 0 ||
           strcmp(uid_dir -> d_name, "..") == 0)
        {
            continue;
        }
        
        // 获取第三方软件包名列表
        FILE * package_list_fp = popen(GET_APPLIST, "r");
        if (package_list_fp == NULL)
        {
            printf(" » 获取软件列表失败！\n");
            return -1;
        }
        
        // 遍历第三方用户软件包名列表
        while (fgets(package_list_line, sizeof(package_list_line), package_list_fp))
        {
            package_list_line[strcspn(package_list_line, "\n")] = 0;
            
            // 拼接软件缓存目录，避免完整遍历user下所有目录
            snprintf(app_cache_dir, sizeof(app_cache_dir), "%s/%s/%s/cache", work_dir, uid_dir -> d_name, package_list_line + 8);
            
            // Check
            if (access(app_cache_dir, F_OK) != 0)
            {
                printf(" » 跳过 %s\n", package_list_line + 8);
                continue;
            }
            
            // 获取缓存大小（兆M）
            cache_size = (int)(GetPathSize(app_cache_dir) / 1024 / 1024);
            
            // 比较大小，如果值小于缓存清理限制大小则跳过
            if (cache_size > ClearCacheSize)
            {
                // 调用白名单检查函数，在清理白名单则跳过
                if (whiteListCheck(whitelist_file, package_list_line + 8) == 1)
                {
                    continue;
                }
                
                char clear_command[strlen(app_cache_dir) + 32];
                snprintf(clear_command, sizeof(clear_command), CLEAR_CACHE, app_cache_dir);
                if (system(clear_command) == 0)
                {
                    clean_size += cache_size; // 记录清理大小
                    printf(" » %s 缓存已清除\n", package_list_line + 8);
                    count++;
                    fflush(stdout);
                }
                else
                {
                    no_count++;
                }
            }
            else
            {
                printf(" » 跳过 %s\n", package_list_line + 8);
                no_count++;
                fflush(stdout);
            }
        }
        if (package_list_fp) pclose(package_list_fp);
    }
    
    if (uid_dir_dp) closedir(uid_dir_dp);
    // 返回总清理大小
    printf(" » 共清理 %d 个软件，%d 个软件无需清理\n", count, no_count);
    return clean_size;
}

/* 
白名单检查函数
接收：
    char * whitelist_file白名单文件
    char * App 软件包名
返回：
    int 找到返回1，未找到返回0，失败返回-1
*/
static int whiteListCheck(char * whitelist_file, char * App)
{
    // 打开白名单文件并遍历查找包名
    int end = 0;
    char package_line[MAX_PACKAGE] = "";
    FILE * whitelist_file_fp = fopen(whitelist_file, "r");
    if (whitelist_file_fp)
    {
        while (fgets(package_line, sizeof(package_line), whitelist_file_fp))
        {
            package_line[strcspn(package_line, "\n")] = 0;
            if (strcmp(package_line, App) == 0)
            {
                end = 1;
            }
        }
        fclose(whitelist_file_fp);
    }
    else
    {
        end = -1;
    }
    return end;
}

/*
获取磁盘占用大小
接收：
    char * path 路径/文件
返回：
    long 大小，单位：字节（Byte）
*/
static int GetPathSize(char * path)
{
    if (access(path, F_OK) != 0)
    {
        return 0;
    }
    
    long size = 0;
    struct dirent * entry;
    struct stat file_stat;
    DIR * path_dp = opendir(path);
    if (path_dp == NULL)
    {
        return 0;
    }
    
    while ((entry = readdir(path_dp)))
    {
        if (strcmp(entry -> d_name, ".") == 0 ||
           strcmp(entry -> d_name, "..") == 0)
        {
            continue;
        }
        
        char dir[strlen(path) + strlen(entry -> d_name) + 2];
        snprintf(dir, sizeof(dir), "%s/%s", path, entry -> d_name);
        
        if (stat(dir, &file_stat) == -1)
        {
            continue;
        }
        if (S_ISDIR(file_stat.st_mode))
        {
            size += GetPathSize(dir);
        }
        else
        {
            size += file_stat.st_size;
        }
    }
    closedir(path_dp);
    return size;
}

static int ClearSystemCache()
{
    if (getuid() != 0)
    {
        printf(" » 请授予root权限！\n");
        return 1;
    }
    
    int count = 0, no_count = 0;
    char app_cache_path[MAX_PACKAGE + 16],
         package_list_line[MAX_PACKAGE] = "";
    
    struct dirent * uid_dir = NULL;
    DIR * uid_dir_dp = opendir(DATA_DIR);
    if (uid_dir_dp == NULL)
    {
        printf(" » 软件目录打开失败！\n");
        return 1;
    }
    
    while ((uid_dir = readdir(uid_dir_dp)))
    {
        if (strcmp(uid_dir -> d_name, ".") == 0 ||
           strcmp(uid_dir -> d_name, "..") == 0)
        {
            continue;
        }
        
        // 遍历清空系统组件cache文件夹
        FILE * package_list = popen(GET_S_APPLIST, "r");
        if (package_list == NULL)
        {
            printf("系统软件列表获取失败\n");
            closedir(uid_dir_dp);
            return 1;
        }
        
        while (fgets(package_list_line, sizeof(package_list_line), package_list))
        {
            package_list_line[strcspn(package_list_line, "\n")] = 0;
            snprintf(app_cache_path, sizeof(app_cache_path), "%s/%s/%s/cache", DATA_DIR, uid_dir -> d_name, package_list_line + 8);
            
            if (access(app_cache_path, F_OK) != 0)
            {
                continue;
            }
            else
            {
                char clear_command[strlen(app_cache_path) + 64];
                snprintf(clear_command, sizeof(clear_command), CLEAR_CACHE, app_cache_path);
                if (system(clear_command) == 0)
                {
                    printf(" » %s 缓存已清除\n", package_list_line + 8);
                    fflush(stdout);
                    count++;
                }
                else
                { 
                    printf(" » 跳过 %s\n", package_list_line + 8);
                    fflush(stdout);
                    no_count++;
                }
            }
        }
        
        pclose(package_list);
        
    }
    closedir(uid_dir_dp);
    
    // 清除“MTP主机”组件数据可解决MTP连接文件显示不全的问题
    system("pm clear com.android.mtp >/dev/null 2>&1");
    // 清空系统缓存
    system("rm -r /cache/* 2>/dev/null");
    system("rm -r /data/resource-cache/* 2>/dev/null");
    system("rm -r /data/system/package_cache/* 2>/dev/null");
    system("rm -r /data/dalvik-cache/* 2>/dev/null");
    
    printf(" » 系统缓存已清空！建议重启系统！\n");
    printf(" » 共清理 %d 个系统软件，%d 个系统软件无需清理\n", count, no_count);
    return 0;
}
