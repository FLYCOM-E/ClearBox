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
        else
        {
            fprintf(stderr, L_OPEN_PATH_FAILED, path, strerror(errno));
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
int whitelist_check(char * whitelist_file, char * package)
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
        fprintf(stderr, L_OPEN_FILE_FAILED, whitelist_file, strerror(errno));
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
long get_path_size(char * path)
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
        fprintf(stderr, L_OPEN_PATH_FAILED, path, strerror(errno));
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
            size += get_path_size(dir);
        }
        else
        {
            size += file_stat.st_size;
        }
    }
    closedir(path_dp);
    return size;
}

/*
通知发送函数，通过切换用户至shell并使用Shell发送通知
接收：
    char * title 通知标题
    char * message 消息内容
返回：
    int 成功返回 0，失败返回 1
*/
int post(char * title, char * message)
{
    //生成一个随机ID
    srand((unsigned int)time(NULL));
    char rand_str[24] = {0};
    snprintf(rand_str, sizeof(rand_str), "%s-%d", title, rand());
    
    pid_t new_pid = fork();
    if (new_pid == -1)
    {
        printf("Post: Fork Error\n");
        return 1;
    }
    if (new_pid == 0)
    {
        setuid(2000);
        execlp("cmd", "cmd", "notification", "post", "-t", title, rand_str, message, NULL);
        _exit(127);
    }
    else
    {
        int end = 0;
        if (waitpid(new_pid, &end, 0) == -1)
        {
            printf("Post: Wait Error\n");
            return 1;
        }
        if (WIFEXITED(end) && WEXITSTATUS(end) != 0)
        {
            printf("Post: exit code error\n");
            return 1;
        }
    }
    return 0;
}

/*
一个Log函数，用于提供一个统一的Log写入接口
接收：
    char * config_dir 模块配置目录，Log会在这里创建
    char * name_id 进程名
    char * text Log信息
*/
int write_log(char * config_dir, char * name_id, char * text)
{
    //获取当前时间（用于log）
    char now_time[64] = "";
    time_t now_time_tmp = time(NULL);
    struct tm * t_tmp = localtime(&now_time_tmp);
    strftime(now_time, sizeof(now_time), "%m-%d %H:%M:%S", t_tmp);
    
    //定义及打开Log文件
    char log_file[strlen(config_dir) + 32];
    snprintf(log_file, sizeof(log_file), "%s/%s", config_dir, LOG_FILE_NAME);
    FILE * log_file_fp = fopen(log_file, "a+");
    if (log_file_fp)
    {
        fprintf(log_file_fp, "[%s] <%s> %s\n", now_time, name_id, text);
        fclose(log_file_fp);
    }
    else
    {
        fprintf(stderr, L_OPEN_FILE_FAILED, log_file, strerror(errno));
    }
    return 0;
}

/*
此函数用于切换至原始命名空间
有些路径访问会出问题，经排查为命名空间引起
*/
int set_name_space(void)
{
    char * name_space_path = "/proc/1/ns/mnt";
    
    int fd = open(name_space_path, O_RDONLY | O_CLOEXEC);
    if (fd == -1)
    {
        fprintf(stderr, "%s Open error\n", name_space_path);
        return -1;
    }
    if (setns(fd, CLONE_NEWNS) == -1)
    {
        fprintf(stderr, "Setns error\n");
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

/*
此函数用于获取模块 Settings 设置信息
接收：
    char * settings_file 设置文件（完整路径）
    char * key 具体 prop
返回：
    int 返回 -1 失败，否则返回具体值。非数字值返回 0
*/
int get_settings_prop(char * settings_file, char * key)
{
    int value = 0;
    char * line_key = NULL;
    char line[SETTINGS_FILE_MAX_LINE] = {0};
    FILE * settings_file_fp = fopen(settings_file, "r");
    if (settings_file_fp == NULL)
    {
        fprintf(stderr, L_OPEN_FILE_FAILED, settings_file, strerror(errno));
        return -1;
    }
    while (fgets(line, sizeof(line), settings_file_fp))
    {
        line[strcspn(line, "\n")] = 0;
        char * line_p = NULL;
        line_key = strtok_r(line, "=", &line_p);
        if (strcmp(line_key, key) == 0)
        {
            value = (int)strtol(strtok_r(NULL, "=", &line_p), NULL, 10);
            break;
        }
    }
    fclose(settings_file_fp);
    return value;
}

/*
此函数用于设置进程名称
接收：
    char * argv[]
    char * new_name 新名称
*/
int set_server_name(char * argv[], char * new_name)
{
    size_t size = strlen(argv[0]);
    if (strlen(new_name) <= size)
    {
        memset(argv[0], 0, size);
        snprintf(argv[0], size, "%s", new_name);
    }
    prctl(PR_SET_NAME, new_name);
    
    return 0;
}

/*
此函数用于替换文件行
接收：
    char * file 目标文件
    char * target_line 目标行
    char * text 替换内容
    int mode 为 1 则模糊匹配（慎）
返回：
    成功返回 0，失败返回 1，未找到返回 2
全局替换
*/
int s_sed(char * file, char * target_line, char * text, int mode)
{
    int found = 0;
    char line[MAX_PATH] = "";
    char tmp_file[strlen(file) + 128];
    snprintf(tmp_file, sizeof(tmp_file), "%s_%ld", file, (time(NULL) + getpid()));
    
    FILE * tmp_fp = fopen(tmp_file, "w");
    FILE * file_fp = fopen(file, "r");
    if (file_fp == NULL)
    {
        fprintf(stderr, L_OPEN_FILE_FAILED, file, strerror(errno));
        return 1;
    }
    if (tmp_fp == NULL)
    {
        fclose(file_fp);
        fprintf(stderr, L_OPEN_FILE_FAILED, tmp_file, strerror(errno));
        return 1;
    }
    
    while (fgets(line, sizeof(line), file_fp))
    {
        line[strcspn(line, "\n")] = 0;
        if (mode == 1)
        {
            if (strstr(line, target_line))
            {
                fprintf(tmp_fp, "%s\n", text);
                found = 1;
            }
            else
            {
                fprintf(tmp_fp, "%s\n", line);
            }
        }
        else
        {
            if (strcmp(line, target_line) == 0)
            {
                fprintf(tmp_fp, "%s\n", text);
                found = 1;
            }
            else
            {
                fprintf(tmp_fp, "%s\n", line);
            }
        }
    }
    
    fclose(tmp_fp);
    fclose(file_fp);
    
    if (rename(tmp_file, file) != 0)
    {
        unlink(tmp_file);
        fprintf(stderr, L_MOVE_ERROR, file, strerror(errno));
        return 1;
    }
    if (found == 0)
    {
        return 2;
    }
    return 0;
}

/*
此函数用于代替外部 chattr 避免进程调用开销
接收：
    char * path 路径
    int mode 模式，1 锁定，0 解锁
    int dir 是否递归，1 递归，0 仅锁定不递归，非目录无效
返回：
    成功返回 0，失败返回 1
*/
int s_chattr(char * path, int mode, int dir)
{
    int fd = open(path, O_RDONLY | O_NONBLOCK | O_NOCTTY | O_NOFOLLOW);
    if (fd < 0)
    {
        return 1;
    }
    unsigned int flags = 0;
    if (ioctl(fd, FS_IOC_GETFLAGS, &flags) < 0)
    {
        close(fd);
        return 1;
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
        return 1;
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
