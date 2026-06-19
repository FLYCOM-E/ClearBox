#include "main.h"

/*
此函数用于代替外部 chattr 避免进程调用开销
接收：
    char * path 路径
    int mode 模式，1 锁定，0 解锁
    int dir 是否递归，1 递归，0 仅锁定不递归，非目录无效
返回：
    成功返回 0，失败返回 -1
*/
int s_chattr(char * path, int mode, int dir)
{
    int fd = open(path, O_RDONLY | O_NONBLOCK | O_NOCTTY | O_NOFOLLOW);
    if (fd < 0)
    {
        return -1;
    }
    unsigned int flags = 0;
    if (ioctl(fd, FS_IOC_GETFLAGS, &flags) < 0)
    {
        close(fd);
        return -1;
    }
    
    if (mode == 1)
    {
        flags |= FS_IMMUTABLE_FL;
    }
    else
    {
        flags &= ~(unsigned int)FS_IMMUTABLE_FL;
    }
    
    if (ioctl(fd, FS_IOC_SETFLAGS, &flags) < 0)
    {
        close(fd);
        return -1;
    }
    close(fd);
    
    if (dir != 1)
    {
        return 0;
    }
    
    DIR * path_dp = opendir(path);
    if (path_dp == NULL)
    {
        return 0;
    }
    struct dirent * entry;
    while ((entry = readdir(path_dp)))
    {
        if (strcmp(entry -> d_name, ".") == 0 ||
            strcmp(entry -> d_name, "..") == 0)
        {
            continue;
        }
        char paths[strlen(path) + strlen(entry -> d_name) + 2];
        snprintf(paths, sizeof(paths), "%s/%s", path, entry -> d_name);
        s_chattr(paths, mode, dir);
    }
    closedir(path_dp);
    
    return 0;
}
