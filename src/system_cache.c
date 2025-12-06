// 此Core来自ClearBox模块，用于清空系统缓存
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PACKAGE 256
#define DATA_DIR "/data/user"
#define GET_S_APPLIST "pm list package -s 2>/dev/null"
#define CLEAR_CACHE "rm -r %s/* 2>/dev/null"

int main()
{
    if (getuid() != 0)
    {
        printf(" » 请授予root权限！\n");
        return 1;
    }
    
    int count = 0, no_count = 0;
    char app_cache_path[256] = "", package_list_line[MAX_PACKAGE] = "";
    
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
                char clear_command[256] = "";
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
    
    printf(" >>> 系统缓存已清空！建议重启系统！\n");
    printf(" >>> 共清理 %d 个系统软件，%d 个系统软件无需清理\n", count, no_count);
    return 0;
}
