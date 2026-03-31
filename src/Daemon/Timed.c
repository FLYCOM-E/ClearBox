// 此 Code 来自ClearBox模块，是crond代替品哦～
#include "../INCLUDE/BashCore.h"

#define WAIT_TIME 60 // 单次循环等待时间，单位秒
#define MAX_CONFIG 512 // 最大配置数量
#define MAX_CONFIG_NAME 256 // 配置文件名称最大长度
#define CONFIG_LINE_MAX_LEN 512 // 配置文件行最大长度
#define MAX_COMMAND_LEN 4096 // 命令最大长度
#define MAX_TITLE_LEN 128 // 通知标题最大长度
#define MAX_MESSAGE_LEN 512 // 通知内容最大长度
#define SERVER_NAME "ClearBox Timed"

static int running(char * command);

struct config_file
{
    // for time
    char time_unit;
    int time_num;
    // for date
    time_t old_time;
    // For post
    int post;
    char title[MAX_TITLE_LEN];
    char message[MAX_MESSAGE_LEN];
    // For in time
    int in;
    int start_hour;
    int end_hour;
    // other
    char run[MAX_COMMAND_LEN];
    char config_name[MAX_CONFIG_NAME];
    time_t last_error_notify;
};

int main(int argc, char * argv[])
{
    if (getuid() != 0)
    {
        fprintf(stderr, L_NOT_USE_ROOT);
        return 1;
    }
    if (argc != 2)
    {
        fprintf(stderr, L_ARGS_FAILED);
        return 1;
    }
    
    // 设置命名空间
    if (set_name_space() != 0)
    {
        return 1;
    }
    
    if (access(argv[1], F_OK) != 0)
    {
        fprintf(stderr, L_CONFIG_PATH_NOTFOUND);
        return 1;
    }
    
    int read_config = 0;
    struct config_file config[MAX_CONFIG]; // 创建结构体
    
    // 获取传入PATH类型
    struct stat path_stat;
    if (lstat(argv[1], &path_stat) == -1)
    {
        fprintf(stderr, L_PATH_STAT_FAILED, argv[1]);
        return 1;
    }
    
    // 目录类型继续解析，否则报错退出
    if (S_ISDIR(path_stat.st_mode))
    {
        struct stat file_stat;
        struct dirent * entry;
        DIR * config_dir_dp = opendir(argv[1]);
        if (config_dir_dp == NULL)
        {
            fprintf(stderr, L_OPEN_PATH_FAILED, argv[1]);
            return 1;
        }
        
        // 遍历配置目录
        while ((entry = readdir(config_dir_dp)))
        {
            if (strcmp(entry -> d_name, ".") == 0 ||
               strcmp(entry -> d_name, "..") == 0)
            {
                continue;
            }
            
            // 拼接完整路径并跳过目录/链接
            size_t path_len = (strlen(argv[1]) + strlen(entry -> d_name) + 2);
            if (path_len > MAX_PATH)
            {
                fprintf(stderr, L_CONFIG_PATH_TOOLONG);
                continue;
            }
            char path[path_len];
            snprintf(path, sizeof(path), "%s/%s", argv[1], entry -> d_name);
            
            if (lstat(path, &file_stat) == -1)
            {
                continue;
            }
            if (S_ISDIR(file_stat.st_mode) || S_ISLNK(file_stat.st_mode))
            {
                continue;
            }
            else
            {
                int line_count = 0; // 行计数
                int time_ = 0, date_ = 0, run_ = 0; // 记录是否解析标志位
                char line[CONFIG_LINE_MAX_LEN] = {0};
                
                FILE * config_fp = fopen(path, "r");
                if (config_fp == NULL)
                {
                    fprintf(stderr, L_OPEN_FILE_FAILED, entry -> d_name, strerror(errno));
                    errno = 0;
                    continue;
                }
                while (fgets(line, sizeof(line), config_fp))
                {
                    // 提取名称/值并匹配读入结构体
                    
                    line_count++;
                    line[strcspn(line, "\n")] = 0;
                    
                    // 解析 key/value
                    char * key = strtok(line, "=");
                    char * value = strtok(NULL, "=");
                    if (value == NULL || key == NULL)
                    {
                        fprintf(stderr, L_TD_LINE_ERR_VALUE, entry -> d_name, line_count, key);
                        continue;
                    }
                    
                    // Case
                    if (strcasecmp(key, "time") == 0)
                    {
                        char * time_str = strtok(value, "/");
                        char * unit_str = strtok(NULL, "/");
                        if (time_str && unit_str &&
                            atoi(time_str) != 0 &&
                            (unit_str[0] == 'D' ||
                            unit_str[0] == 'H' ||
                            unit_str[0] == 'M'))
                        {
                            config[read_config].time_unit = unit_str[0];
                            config[read_config].time_num = atoi(time_str);
                        }
                        else
                        {
                            fprintf(stderr, L_TD_LINE_ERR_VALUE, entry -> d_name, line_count, key);
                            continue;
                        }
                        time_ = 1;
                    }
                    else if (strcasecmp(key, "date") == 0)
                    {
                        // 这里不做检查，如果出错则默认为当前时间
                        config[read_config].old_time = (time_t)atol(value);
                        if (config[read_config].old_time <= 0)
                        {
                            config[read_config].old_time = time(NULL);
                        }
                        date_ = 1;
                    }
                    else if (strcasecmp(key, "run") == 0)
                    {
                        // 不检查命令（不好做检查）
                        if (strlen(value) >= MAX_COMMAND_LEN)
                        {
                            fprintf(stderr, L_TD_LINE_ERR_VALUE, entry -> d_name, line_count, key);
                            continue;
                        }
                        snprintf(config[read_config].run, sizeof(config[read_config].run), "%s", value);
                        run_ = 1;
                    }
                    else if (strcasecmp(key, "in") == 0)
                    {
                        config[read_config].start_hour = 0;
                        config[read_config].end_hour = 0;
                        
                        char * start_tm_str = strtok(value, "/");
                        char * end_tm_str = strtok(NULL, "/");
                        
                        if (start_tm_str && end_tm_str)
                        {
                            int start_hour = 0, end_hour = 0;
                            start_hour = atoi(start_tm_str);
                            end_hour = atoi(end_tm_str);
                            
                            if (start_hour > 23 ||
                                start_hour < 0 ||
                                end_hour > 23 ||
                                end_hour < 0)
                            {
                                fprintf(stderr, L_TD_LINE_ERR_VALUE, entry -> d_name, line_count, key);
                                continue;
                            }
                            
                            config[read_config].start_hour = start_hour;
                            config[read_config].end_hour = end_hour;
                            config[read_config].in = 1;
                        }
                        else
                        {
                            fprintf(stderr, L_TD_LINE_ERR_VALUE, entry -> d_name, line_count, key);
                            continue;
                        }
                    }
                    else if (strcasecmp(key, "post") == 0)
                    {
                        char * message = strchr(value, '/');
                        char * title = strtok(value, "/");
                        if (title && message)
                        {
                            if (strlen(title) > MAX_TITLE_LEN)
                            {
                                fprintf(stderr, L_TD_W_POST_TITLE_TOOLONG, entry -> d_name);
                            }
                            if (strlen(message + 1) > MAX_MESSAGE_LEN)
                            {
                                fprintf(stderr, L_TD_W_POST_MESSAGE_TOOLONG, entry -> d_name);
                            }
                            snprintf(config[read_config].title, sizeof(config[read_config].title), "%s", title);
                            snprintf(config[read_config].message, sizeof(config[read_config].message), "%s", message + 1);
                            config[read_config].post = 1;
                        }
                        else
                        {
                            fprintf(stderr, L_TD_LINE_ERR_VALUE, entry -> d_name, line_count, key);
                            continue;
                        }
                    }
                    else
                    {
                        fprintf(stderr, L_TD_LINE_ERR_KEY, entry -> d_name, line_count, key);
                    }
                }
                fclose(config_fp);
                
                if (time_ != 1 ||
                    date_ != 1 ||
                    run_ != 1)
                {
                    fprintf(stderr, L_TD_CONFIG_ERROR, entry -> d_name);
                    continue; // 跳过，read_config 不自增，后面解析自然覆盖
                }
                
                // 保留配置名称
                snprintf(config[read_config].config_name, sizeof(config[read_config].config_name), "%s", entry -> d_name);
                
                read_config += 1;
                if (read_config == MAX_CONFIG) // 限制配置数量
                {
                    printf(L_TD_MAX_CONFIG, MAX_CONFIG);
                    break;
                }
                printf(L_TD_CONFIG_SUCCESS, entry -> d_name);
            }
        }
        closedir(config_dir_dp);
        if (read_config == 0)
        {
            fprintf(stderr, L_NOCONFIG);
            return 1;
        }
    }
    else
    {
        fprintf(stderr, L_PATH_NOTISDIR, argv[1]);
        return 1;
    }
    
    // Daemon
    pid_t new_pid = fork();
    if (new_pid == -1)
    {
        fprintf(stderr, L_SERVER_START_ERR);
        return 1;
    }
    if (new_pid != 0)
    {
        exit(0);
    }
    setsid();
    chdir("/");
    int fd = open("/dev/null", O_RDWR);
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    close(fd);
    
    // Post
    char start_success_str[256] = {0}; // 对应宏内容不能超过此大小
    snprintf(start_success_str, sizeof(start_success_str), L_TD_START_SUCCESS, read_config);
    post(SERVER_NAME, start_success_str);
    
    for ( ; ; )
    {
        time_t now_time = time(NULL);
        struct tm now_time_local;
        localtime_r(&now_time, &now_time_local);
        
        int i = 0;
        while (i < read_config)
        {
            int run = 0;
            if (config[i].in == 1)
            {
                if (!(now_time_local.tm_hour >= config[i].start_hour &&
                    now_time_local.tm_hour <= config[i].end_hour))
                {
                    i++;
                    continue;
                }
            }
            switch (config[i].time_unit) // 匹配配置对应时间单位判断是否需要执行
            {
                case 'D':
                    // day
                    if (difftime(now_time, config[i].old_time) >= (config[i].time_num * 86400))
                    {
                        run = 1;
                    }
                    break;
                case 'H':
                    // hour
                    if (difftime(now_time, config[i].old_time) >= (config[i].time_num * 3600))
                    {
                        run = 1;
                    }
                    break;
                case 'M':
                    // minute
                    if (difftime(now_time, config[i].old_time) >= (config[i].time_num * 60))
                    {
                        run = 1;
                    }
                    break;
                default:
                    break;
            }
            if (run == 1) // 执行并更新
            {
                running(config[i].run);
                // 如果有设置通知则发送
                if (config[i].post == 1)
                {
                    post(config[i].title, config[i].message);
                }
                
                // 配置文件
                char config_file[strlen(argv[1]) + strlen(config[i].config_name) + 2];
                snprintf(config_file, sizeof(config_file), "%s/%s", argv[1], config[i].config_name);
                
                /* 
                回写
                如果配置存在则更新，否则不重新创建，这是为了配合前端
                删掉配置即停用的设置。重启进程前配置仍然生效
                */
                if (access(config_file, F_OK) == 0)
                {
                    errno = 0;
                    FILE * config_file_fp = fopen(config_file, "w");
                    if (config_file_fp)
                    {
                        if (config[i].post == 1 && config[i].in == 1)
                        {
                            fprintf(config_file_fp, "time=%d/%c\ndate=%ld\nrun=%s\npost=%s/%s\nin=%d/%d",
                                    // time
                                    config[i].time_num, config[i].time_unit,
                                    // date
                                    now_time,
                                    // run
                                    config[i].run,
                                    // post
                                    config[i].title, config[i].message,
                                    // in
                                    config[i].start_hour, config[i].end_hour
                                  );
                        }
                        else if (config[i].post == 1 && config[i].in == 0)
                        {
                            fprintf(config_file_fp, "time=%d/%c\ndate=%ld\nrun=%s\npost=%s/%s",
                                    // time
                                    config[i].time_num, config[i].time_unit,
                                    // date
                                    now_time,
                                    // run
                                    config[i].run,
                                    // post
                                    config[i].title, config[i].message
                                  );
                        }
                        else if (config[i].post == 0 && config[i].in == 1)
                        {
                            fprintf(config_file_fp, "time=%d/%c\ndate=%ld\nrun=%s\nin=%d/%d",
                                    // time
                                    config[i].time_num, config[i].time_unit,
                                    // date
                                    now_time,
                                    // run
                                    config[i].run,
                                    // in
                                    config[i].start_hour, config[i].end_hour
                                  );
                        }
                        else
                        {
                            fprintf(config_file_fp, "time=%d/%c\ndate=%ld\nrun=%s",
                                    // time
                                    config[i].time_num, config[i].time_unit,
                                    // date
                                    now_time,
                                    // run
                                    config[i].run
                                  );
                        }
                        fclose(config_file_fp);
                    }
                    // Check Errno
                    if (errno != 0 &&
                        difftime(now_time, config[i].last_error_notify) >= 3600)
                    {
                        char error_text[512] = {0};
                        snprintf(error_text, sizeof(error_text), L_TD_CONFIG_WRITE_ERROR,
                                 config[i].config_name,
                                 strerror(errno));
                        post(SERVER_NAME, error_text);
                        config[i].last_error_notify = now_time; // 记录上次通知时间，避免短时间多次通知
                    }
                }
                
                // 更新时间戳
                config[i].old_time = now_time;
            }
            i++;
        }
        
        // 回收子进程
        while (waitpid(-1, NULL, WNOHANG) > 0);
        sleep((unsigned int)(60 - (time(NULL) % 60)));
    }
}

static int running(char * command)
{
    pid_t new_pid = fork();
    if (new_pid == -1)
    {
        return 1;
    }
    if (new_pid == 0)
    {
        execl("/bin/sh", "sh", "-c", command, NULL);
        _exit(errno);
    }
    // 这里不等待子进程，避免阻塞
    return 0;
}
