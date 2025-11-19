// 此Core来自ClearBox模块，用于清空内部储存软件缓存
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>

#define data_dir "/data/user"
#define WHITELIST_FILE "ClearWhitelist.prop"

static int wipeCache(char * work_dir, char * whitelist_file, int ClearCacheSize);
static int whiteListCheck(char * whitelist_file, char * App);

int main()
{
    if (getuid() != 0)
    {
        printf(" » 请授予root权限！");
        return 1;
    }
    if (system("ClearBox -v >/dev/null 2>&1") != 0)
    {
        printf(" » 模块加载异常，请排查反馈！\n");
        return 1;
    }
    
    // work_dir定义
    char work_dir[64] = "";
    FILE * work_dir_fp = popen("ClearBox -w 2>/dev/null", "r");
    if (work_dir_fp)
    {
        fgets(work_dir, sizeof(work_dir), work_dir_fp);
        pclose(work_dir_fp);
        work_dir[strcspn(work_dir, "\n")] = 0;
    }
    
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
    char whitelist_file[64] = "";
    snprintf(whitelist_file, sizeof(whitelist_file), "%s/%s", work_dir, WHITELIST_FILE);
    
    /* 
    读取：
    ClearCacheSize（缓存清理限制大小）
    cleardisk（是否清理SD软件缓存）设置值
    */
    char * key_len_fp = NULL;
    int ClearCacheSize = 0, cleardisk = 0;
    char settings_file[strlen(work_dir) + 32], key_len[32] ="";
    settings_file[0] = '\n';
    snprintf(settings_file, sizeof(settings_file), "%s/settings.prop", work_dir);
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
    int clear_size = wipeCache(data_dir, whitelist_file, ClearCacheSize);
    printf(" >>> 内部储存软件缓存删除完成\n >>> 共清理：%d兆\n", clear_size);
    // cleardisk = 1：允许清理拓展SD缓存
    if (cleardisk == 1)
    {
        if (access(micro_dir, F_OK) == 0)
        {
            clear_size = wipeCache(micro_dir, whitelist_file, ClearCacheSize);
            printf(" >>> 外部储存软件缓存删除完成\n >>> 共清理：%d兆\n", clear_size);
        }
    }
    
    return 0;
    
}

/* 
此函数用于清理软件缓存，返回总清理大小，接收参数：
char work_dir
    软件数据目录，这里统一使用xxx/user，自动处理可能的多用户情况，兼容拓展SD
char whitelist_file
    白名单文件（完整路径）
int ClearCacheSize
    缓存清理限制大小
*/
static int wipeCache(char * work_dir, char * whitelist_file, int ClearCacheSize)
{
    // 定义所需变量
    int cache_size = 0, clean_size = 0, count = 0, no_count = 0;
    char cache_size_char[16] = "", app_cache_dir[256] = "", package_list_line[256] = "";
    
    // 打开user目录
    struct dirent * uid_dir;
    DIR * uid_dir_dp = opendir(work_dir);
    if (uid_dir_dp == NULL)
    {
        printf(" » 目录打开失败！\n");
        return 0;
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
        FILE * package_list_fp = popen("pm list package -3 2>/dev/null", "r");
        if (package_list_fp == NULL)
        {
            printf(" » 获取软件列表失败！\n");
            return 1;
        }
        
        // 遍历第三方用户软件包名列表
        while (fgets(package_list_line, sizeof(package_list_line), package_list_fp))
        {
            package_list_line[strcspn(package_list_line, "\n")] = 0;
            
            // 拼接软件缓存目录，避免完整遍历user下所有目录
            snprintf(app_cache_dir, sizeof(app_cache_dir), "%s/%s/%s/cache", work_dir, uid_dir -> d_name, package_list_line + 8);
            
            // 本来不打算在这里检查，毕竟下面du就有检查目录是否存在的作用
            // 但发现用du合并检查会有莫名的漏网之鱼（（
            if (access(app_cache_dir, F_OK) != 0)
            {
                continue;
            }
            
            // 调用du命令获取软件缓存目录大小（单位：兆）
            char get_cache_size[strlen(app_cache_dir) + 32];
            get_cache_size[0] = '\0';
            snprintf(get_cache_size, sizeof(get_cache_size), "du -s -m %s 2>/dev/null", app_cache_dir);
            FILE * CacheSize_fp = popen(get_cache_size, "r");
            if (CacheSize_fp == NULL)
            {
                continue;
            }
            fgets(cache_size_char, sizeof(cache_size_char), CacheSize_fp);
            pclose(CacheSize_fp);
            cache_size_char[strcspn(cache_size_char, "\t")] = 0;
            
            // 将char类型缓存大小转换为int
            cache_size = atoi(cache_size_char);
            
            // 比较大小，如果值小于缓存清理限制大小则跳过
            if (cache_size > ClearCacheSize)
            {
                // 调用白名单检查函数，在清理白名单则跳过
                if (whiteListCheck(whitelist_file, package_list_line + 8) == 0)
                {
                    char clear_command[strlen(app_cache_dir) + 32];
                    clear_command[0] = '\0';
                    snprintf(clear_command, sizeof(clear_command), "rm -r %s/* >/dev/null 2>&1", app_cache_dir);
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
    printf(" >>> 共清理 %d 个软件，%d 个软件无需清理\n", count, no_count);
    return clean_size;
}

/* 
白名单检查函数，接收白名单文件路径、软件包名
返回类型：int
    1代表在白名单中找到匹配项
*/
static int whiteListCheck(char * whitelist_file, char * App)
{
    // 打开白名单文件并遍历查找包名
    char package_line[64] = "";
    FILE * whitelist_file_fp = fopen(whitelist_file, "r");
    if (whitelist_file_fp)
    {
        while (fgets(package_line, sizeof(package_line), whitelist_file_fp))
        {
            package_line[strcspn(package_line, "\n")] = 0;
            if (strcmp(package_line, App) == 0)
            {
                return 1;
            }
        }
        fclose(whitelist_file_fp);
    }
    return 0;
}
