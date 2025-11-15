// 此Core来自ClearBox模块，用于清空系统缓存
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <stdlib.h>

#define data_dir "/data/user"

int main()
{
    uid_t nowUid = getuid();
    if (nowUid != 0)
    {
        printf(" » 请授予root权限！\n");
        return 1;
    }
    
    int count = 0, noCount = 0;
    char path[256] = "", package[128] = "";
    
    struct dirent * uid_dir = NULL;
    DIR * userid_dir = opendir(data_dir);
    if (userid_dir == NULL)
    {
        printf(" » 软件目录打开失败！\n");
        return 1;
    }
    
    while ((uid_dir = readdir(userid_dir)))
    {
        if (strcmp(uid_dir -> d_name, ".") == 0 ||
           strcmp(uid_dir -> d_name, "..") == 0)
        {
            continue;
        }
        
        // 遍历清空系统组件cache文件夹
        FILE * packageList = popen("pm list package -s 2>/dev/null", "r");
        if (packageList == NULL)
        {
            printf("系统软件列表获取失败\n");
            closedir(userid_dir);
            return 1;
        }
        
        while (fgets(package, sizeof(package), packageList))
        {
            package[strcspn(package, "\n")] = 0;
            snprintf(path, sizeof(path), "%s/%s/%s/cache", data_dir, uid_dir -> d_name, package + 8);
            
            if (access(path, F_OK) != 0)
            {
                continue;
            }
            else
            {
                char clearCommand[256] = "";
                
                snprintf(clearCommand, sizeof(clearCommand), "rm -r %s/* 2>/dev/null", path);
                if (system(clearCommand) == 0)
                {
                    printf(" » %s 缓存已清除\n", package + 8);
                    fflush(stdout);
                    count++;
                }
                else
                { 
                    printf(" » 跳过 %s\n", package + 8);
                    fflush(stdout);
                    noCount++;
                }
            }
        }
        
        pclose(packageList);
        
    }
    closedir(userid_dir);
    
    // 清除“MTP主机”组件数据可解决MTP连接文件显示不全的问题
    system("pm clear com.android.mtp >/dev/null 2>&1");
    
    // 清空系统缓存
    system("rm -r /cache/* 2>/dev/null");
    system("rm -r /data/resource-cache/* 2>/dev/null");
    system("rm -r /data/system/package_cache/* 2>/dev/null");
    system("rm -r /data/dalvik-cache/* 2>/dev/null");
    
    printf(" >>> 系统缓存已清空！建议重启系统！\n");
    printf(" >>> 共清理 %d 个系统软件，%d 个系统软件无需清理\n", count, noCount);
    
    return 0;
}
