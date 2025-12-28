#include "BashCore.h"

/*
递归删除函数
传入：
    char * path
    int all 是否删除目录本身，1删除，0跳过
返回：
    long 成功返回清理大小（字节），失败/为空返回0
*/
long s_remove(char * path, int all)
{
    if (access(path, F_OK) != 0)
    {
        return 0;
    }
    
    long size_byte = 0;
    struct stat file_stat;
    if (lstat(path, &file_stat) == -1)
    {
        return 0;
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
