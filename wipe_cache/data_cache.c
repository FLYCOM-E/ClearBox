// 此Core来自ClearBox模块，用于清空内部储存软件缓存
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>

static int WipeCache(char * workDir, char * whiteList, int ClearCacheSize);
static int whiteListcheck(char * whiteList, char * App);

int main()
{
    uid_t nowUid = getuid();
    if (nowUid != 0)
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
    fgets(work_dir, sizeof(work_dir), work_dir_fp);
    fclose(work_dir_fp);
    work_dir[strcspn(work_dir, "\n")] = 0;
    
    // data_dir定义
    char data_dir[64] = "/data/user";
    
    // micro_dir定义
    char cardname[64] = "", micro_dir[128] = "";
    FILE * cardname_fp = popen("ls /mnt/expand/ | cut -f1 -d ' ' 2>/dev/null", "r");
    fgets(cardname, sizeof(cardname), cardname_fp);
    pclose(cardname_fp);
    cardname[strcspn(cardname, "\n")] = 0;
    snprintf(micro_dir, sizeof(micro_dir), "/mnt/expand/%s/user", cardname);
    
    // whiteList定义
    char whiteList[64] = "";
    snprintf(whiteList, sizeof(whiteList), "%s/ClearWhitelist.prop", work_dir);
    
    /* 读取：
    ClearCacheSize（缓存清理限制大小）
    cleardisk（是否清理SD软件缓存）设置值
    */
    char * key_fp = NULL;
    int ClearCacheSize = 0, cleardisk = 0;
    char settingsFile[strlen(work_dir) + 32], key[32] ="";
    settingsFile[0] = '\n';
    snprintf(settingsFile, sizeof(settingsFile), "%s/settings.prop", work_dir);
    FILE * settingsFile_fp = fopen(settingsFile, "r");
    if (settingsFile_fp)
    {
        while (fgets(key, sizeof(key), settingsFile_fp))
        {
            key[strcspn(key, "\n")] = 0;
            if (strstr(key, "ClearCacheSize="))
            {
                key_fp = strtok(key, "=");
                key_fp = strtok(NULL, "=");
                ClearCacheSize = atoi(key_fp);
            }
            if (strstr(key, "cleardisk="))
            {
                key_fp = strtok(key, "=");
                key_fp = strtok(NULL, "=");
                cleardisk = atoi(key_fp);
            }
        }
        fclose(settingsFile_fp);
    }
    
    //调用处理函数
    int C_Size = WipeCache(data_dir, whiteList, ClearCacheSize);
    printf(" >>> 内部储存软件缓存删除完成\n >>> 共清理：%d兆\n", C_Size);
    // cleardisk = 1：允许清理拓展SD缓存
    if (cleardisk == 1)
    {
        if (access(micro_dir, F_OK) == 0)
        {
            C_Size = WipeCache(micro_dir, whiteList, ClearCacheSize);
            printf(" >>> 外部储存软件缓存删除完成\n >>> 共清理：%d兆\n", C_Size);
        }
    }
    
    return 0;
    
}

/* 
此函数用于清理软件缓存，返回总清理大小，接收参数：
char workDir
    软件数据目录，这里统一使用xxx/user，自动处理可能的多用户情况，兼容拓展SD
char whiteList
    白名单文件（完整目录）
int ClearCacheSize
    缓存清理限制大小
*/
static int WipeCache(char * workDir, char * whiteList, int ClearCacheSize)
{
    // 定义所需变量
    int CacheSize = 0, CleanSize = 0;
    char CacheSize_C[16] = "", AppDir[128] = "", packageList[128] = "", command[128] = "", clearCommand[128] = "";
    
    // 打开user目录
    struct dirent * uiddir;
    DIR * uiddir_fp = opendir(workDir);
    if (uiddir_fp == NULL)
    {
        printf(" » 目录打开失败！\n");
        return 0;
    }
    
    // while遍历user目录（处理多用户
    while ((uiddir = readdir(uiddir_fp)))
    {
        if (strcmp(uiddir -> d_name, ".") == 0 ||
           strcmp(uiddir -> d_name, "..") == 0)
        {
            continue;
        }
        
        // 获取第三方软件包名列表
        FILE * packageList_fp = popen("pm list package -3 2>/dev/null", "r");
        if (packageList_fp == NULL)
        {
            printf(" » 获取软件列表失败！\n");
            return 0;
        }
        
        // 遍历第三方用户软件包名列表
        while (fgets(packageList, sizeof(packageList), packageList_fp))
        {
            packageList[strcspn(packageList, "\n")] = 0;
            
            // 拼接软件缓存目录，避免完整遍历user下所有目录
            snprintf(AppDir, sizeof(AppDir), "%s/%s/%s/cache", workDir, uiddir -> d_name, packageList + 8);
            
            // 本来不打算在这里检查，毕竟下面du就有检查目录是否存在的作用
            // 但发现用du合并检查会有莫名的漏网之鱼（（
            if (access(AppDir, F_OK) != 0)
            {
                continue;
            }
            
            // 调用du命令获取软件缓存目录大小（单位：兆）
            snprintf(command, sizeof(command), "du -s -m %s 2>/dev/null", AppDir);
            FILE * CacheSize_fp = popen(command, "r");
            if (CacheSize_fp == NULL)
            {
                continue;
            }
            fgets(CacheSize_C, sizeof(CacheSize_C), CacheSize_fp);
            pclose(CacheSize_fp);
            CacheSize_C[strcspn(CacheSize_C, "\t")] = 0;
            
            // 将char类型缓存大小转换为int
            CacheSize = atoi(CacheSize_C);
            
            // 比较大小，如果值小于缓存清理限制大小则跳过
            if (CacheSize > ClearCacheSize)
            {
                // 调用白名单检查函数，在清理白名单则跳过
                if (whiteListcheck(whiteList, packageList + 8) == 0)
                {
                    snprintf(clearCommand, sizeof(clearCommand), "rm -r %s/* >/dev/null 2>&1", AppDir);
                    if (system(clearCommand) == 0)
                    {
                        CleanSize += CacheSize; // 记录清理大小
                        printf(" » %s 缓存已清除\n", packageList + 8);
                        fflush(stdout);
                    }
                }
            }
            else
            {
                printf(" » 跳过 %s\n", packageList + 8);
                fflush(stdout);
            }
        }
        if (packageList_fp) pclose(packageList_fp);
    }
    
    if (uiddir_fp) closedir(uiddir_fp);
    // 返回总清理大小
    return CleanSize;
}

/* 
白名单检查函数，接收白名单文件路径、软件包名
返回类型：int
    1代表在白名单中找到匹配项
*/
static int whiteListcheck(char * whiteList, char * App)
{
    // 打开白名单文件并遍历查找包名
    char package[64] = "";
    FILE * whiteList_fp = fopen(whiteList, "r");
    if (whiteList_fp)
    {
        while (fgets(package, sizeof(package), whiteList_fp))
        {
            package[strcspn(package, "\n")] = 0;
            if (strcmp(package, App) == 0)
            {
                return 1;
            }
        }
        fclose(whiteList_fp);
    }
    return 0;
}
