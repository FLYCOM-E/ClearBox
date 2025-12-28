#include "BashCore.h"

/*
递归删除函数
传入：
    char * path
    int all 是否删除目录本身，1删除，0跳过
返回：
    long 成功返回清理大小（字节），失败返回-1
*/
long s_remove(char * path, int all)
{
    if (access(path, F_OK) != 0)
    {
        return -1;
    }
    long size_byte = 0;
    struct stat file_stat;
    if (lstat(path, &file_stat) == -1)
    {
        return -1;
    }
    
    if (S_ISDIR(file_stat.st_mode))
    {
        struct dirent * entry;
        DIR * path_dp = opendir(path);
        if (path_dp)
        {
            while ((entry = readdir(path_dp)))
            {
                if (strcmp(entry -> d_name, ".") == 0 ||
                   strcmp(entry -> d_name, "..") == 0)
                {
                    continue;
                }
                char paths[MAX_PATH] = "";
                snprintf(paths, sizeof(paths), "%s/%s", path, entry -> d_name);
                size_byte += s_remove(paths, 1);
            }
            closedir(path_dp);
            if (all == 1)
            {
                remove(path);
            }
        }
    }
    else
    {
        if (remove(path) == 0)
        {
            size_byte += file_stat.st_size;
        }
    }
    return size_byte;
}

/* 
白名单检查函数
接收：
    char * whitelist_file白名单文件
    char * App 软件包名
返回：
    int 找到返回1，未找到返回0，失败返回-1
*/
int whiteListCheck(char * whitelist_file, char * package)
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
            if (strcmp(package_line, package) == 0)
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
long GetPathSize(char * path)
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
        
        if (lstat(dir, &file_stat) == -1)
        {
            continue;
        }
        if (S_ISLNK(file_stat.st_mode))
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
