// 此Code来自ClearBox模块，用于清空内部储存软件缓存
#include "INCLUDE/BashCore.h"

#define MAX_APPLIST 3000
#define DATA_DIR "/data/user" //Max Size 10
#define WHITELIST_FILE "ClearWhitelist.prop" //Max Size 30
#define SETTINGS_FILE "settings.prop" //Max Size 30
#define GET_CARD_ID "ls /mnt/expand/ | cut -f1 -d ' ' 2>/dev/null"
#define GET_APPLIST "cmd package list package -3 2>/dev/null"
#define GET_S_APPLIST "cmd package list package -s 2>/dev/null"

static int wipeCache(char * work_dir, char * whitelist_file, int ClearCacheSize);
static int ClearSystemCache();

int main(int argc, char * argv[])
{
    if (getuid() != 0)
    {
        fprintf(stderr, L_NOT_USE_ROOT);
        return 1;
    }
    
    argc--;
    argv++;
    if (argc < 3)
    {
        fprintf(stderr, L_ARGS_FAILED);
        return 1;
    }
    
    char * work_dir = NULL;
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
        else if (strcmp(argv[0], "-m") == 0)
        {
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
    if (mode == NULL)
    {
        fprintf(stderr, L_ARG_MODE_ERR);
        return 1;
    }
    
    // Case The Mode
    if (strcmp(mode, "AppCache_3") == 0)
    {
        // micro_dir定义
        char card_id[64] = "", micro_dir[128] = "";
        FILE * card_id_fp = popen(GET_CARD_ID, "r");
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
        char * key = NULL;
        char * value = NULL;
        int ClearCacheSize = 0, cleardisk = 0;
        char settings_file[strlen(work_dir) + 32], line[256] = "";
        
        snprintf(settings_file, sizeof(settings_file), "%s/%s", work_dir, SETTINGS_FILE);
        FILE * settings_file_fp = fopen(settings_file, "r");
        if (settings_file_fp)
        {
            // 目标读取清理限制大小/是否清理外部储存
            while (fgets(line, sizeof(line), settings_file_fp))
            {
                key = strtok(line, "=");
                value = strtok(NULL, "=");
                
                if (strcmp(key, "ClearCacheSize") == 0)
                {
                    ClearCacheSize = atoi(value);
                }
                else if (strcmp(key, "cleardisk") == 0)
                {
                    cleardisk = atoi(value);
                }
            }
            fclose(settings_file_fp);
        }
        
        //调用处理函数
        int clear_size = wipeCache(DATA_DIR, whitelist_file, ClearCacheSize);
        if (clear_size == -1)
        {
            fprintf(stderr, L_CC_CLEAR_FAILED);
        }
        else
        {
            printf(L_CC_CLEAR_SUCCESSFUL, clear_size);
        }
        // cleardisk = 1：允许清理拓展SD缓存
        if (cleardisk == 1)
        {
            if (access(micro_dir, F_OK) == 0)
            {
                clear_size = wipeCache(micro_dir, whitelist_file, ClearCacheSize);
                if (clear_size == -1)
                {
                    fprintf(stderr, L_CC_CLEAR_FAILED_SD);
                }
                else
                {
                    printf(L_CC_CLEAR_SUCCESSFUL_SD, clear_size);
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
        fprintf(stderr, L_ARGS_FAILED_2);
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
    char app_cache_dir[512] = {0};
    
    // 获取第三方软件包名列表并储存
    int app_count = 0;
    char package_list[MAX_APPLIST][MAX_PACKAGE];
    FILE * package_list_fp = popen(GET_APPLIST, "r");
    if (package_list_fp == NULL)
    {
        fprintf(stderr, L_GET_APPLIST_ERROR);
        return -1;
    }
    else
    {
        while (fgets(package_list[app_count], sizeof(package_list[app_count]), package_list_fp))
        {
            package_list[app_count][strcspn(package_list[app_count], "\n")] = 0;
            app_count++;
        }
        pclose(package_list_fp);
    }
    
    // 打开user目录
    struct dirent * uid_dir;
    DIR * uid_dir_dp = opendir(work_dir);
    if (uid_dir_dp == NULL)
    {
        fprintf(stderr, L_OPEN_PATH_FAILED, work_dir);
        return -1;
    }
    
    // while遍历user目录（处理多用户
    while ((uid_dir = readdir(uid_dir_dp)))
    {
        if (strcmp(uid_dir -> d_name, ".") == 0 ||
           strcmp(uid_dir -> d_name, "..") == 0 ||
           strspn(uid_dir -> d_name, "0123456789") != strlen(uid_dir -> d_name))
        {
            continue;
        }
        
        // 遍历第三方用户软件包名列表
        for (int i = 0; i < app_count; i++)
        {
            if (strlen(package_list[i]) < 9)
            {
                continue;
            }
            
            // 拼接软件缓存目录，避免完整遍历user下所有目录
            snprintf(app_cache_dir, sizeof(app_cache_dir), "%s/%s/%s/cache", work_dir, uid_dir -> d_name, package_list[i] + 8);
            // Check
            if (access(app_cache_dir, F_OK) != 0)
            {
                continue;
            }
            
            // 获取缓存大小（兆M）
            cache_size = (int)(GetPathSize(app_cache_dir) / 1024 / 1024);
            // 比较大小，如果值小于缓存清理限制大小则跳过
            if (cache_size > ClearCacheSize)
            {
                // 调用白名单检查函数，在清理白名单则跳过
                if (whiteListCheck(whitelist_file, package_list[i] + 8) == 1)
                {
                    continue;
                }
                if (s_remove(app_cache_dir, 0) != -1)
                {
                    count++;
                    clean_size += cache_size; 
                    /* 记录清理大小，本来s_remove实现也会返回大小
                    但上面的大小判断注定这个会浪费 */
                    printf(L_CC_CLEAR, package_list[i] + 8);
                }
                else
                {
                    no_count++;
                }
            }
            else
            {
                no_count++;
                printf(L_CC_CLEAR_SKIP, package_list[i] + 8);
            }
            fflush(stdout);
        }
    }
    closedir(uid_dir_dp);
    // 返回总清理大小
    printf(L_CC_CLEAR_APPCACHE_DONE, count, no_count);
    return clean_size;
}

/* 
此函数用于清理系统缓存
返回：
    int 成功返回0，失败返回-1
*/
static int ClearSystemCache()
{
    char app_cache_path[MAX_PACKAGE + 16],
         package_list_line[MAX_PACKAGE] = "";
    
    struct dirent * uid_dir = NULL;
    DIR * uid_dir_dp = opendir(DATA_DIR);
    if (uid_dir_dp == NULL)
    {
        fprintf(stderr, L_OPEN_PATH_FAILED, DATA_DIR);
        return -1;
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
            fprintf(stderr, L_GET_APPLIST_ERROR);
            closedir(uid_dir_dp);
            return -1;
        }
        
        while (fgets(package_list_line, sizeof(package_list_line), package_list))
        {
            if (strlen(package_list_line) < 9)
            {
                continue;
            }
            package_list_line[strcspn(package_list_line, "\n")] = 0;
            snprintf(app_cache_path, sizeof(app_cache_path), "%s/%s/%s/cache", DATA_DIR, uid_dir -> d_name, package_list_line + 8);
            
            if (access(app_cache_path, F_OK) != 0)
            {
                continue;
            }
            else
            {
                if (s_remove(app_cache_path, 0) != -1)
                {
                    printf(L_CC_CLEAR, package_list_line + 8);
                    fflush(stdout);
                }
            }
        }
        pclose(package_list);
    }
    closedir(uid_dir_dp);
    
    // 清除“MTP主机”组件数据可解决MTP连接文件显示不全的问题
    system("pm clear com.android.mtp >/dev/null 2>&1");
    // 清空系统缓存
    s_remove("/cache", 0);
    s_remove("/data/resource-cache", 0);
    s_remove("/data/system/package_cache", 0);
    s_remove("/data/dalvik-cache", 0);
    
    printf(L_CC_CLEAR_SYSTEMCACHE);
    return 0;
}
