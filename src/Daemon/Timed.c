// 此 Code 来自ClearBox模块，是crond代替品哦～
#include "../INCLUDE/BashCore.h"

#define WAIT_TIME 60
#define MAX_CONFIG 64
#define MAX_CONFIG_NAME 256
#define CONFIG_LINE_MAX_LEN 512
#define MAX_COMMAND_LEN 1024
#define MAX_COMMAND_ARGS 32
#define MAX_TITLE_LEN 128
#define MAX_MESSAGE_LEN 512
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
    // other
    char run[MAX_COMMAND_LEN];
    char config_name[MAX_CONFIG_NAME];
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
            char path[strlen(argv[1]) + strlen(entry -> d_name) + 2];
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
                    fprintf(stderr, L_OPEN_PATH_FAILED, entry -> d_name);
                    continue;
                }
                while (fgets(line, sizeof(line), config_fp))
                {
                    line_count++;
                    line[strcspn(line, "\n")] = 0;
                    
                    // 提取名称/值并匹配读入结构体
                    char * key = strtok(line, "=");
                    char * value = strtok(NULL, "=");
                    
                    // 这里仅检查 VALUE，但其实有检查 KEY 的作用
                    if (value == NULL)
                    {
                        fprintf(stderr, L_TD_LINE_ERR_VALUE, entry -> d_name, line_count, key);
                        continue;
                    }
                    
                    // Case
                    if (strcmp(key, "time") == 0)
                    {
                        char * time_str = strtok(value, "/");
                        char * unit_str = strtok(NULL, "/");
                        if (time_str && unit_str)
                        {
                            config[read_config].time_num = atoi(time_str);
                            config[read_config].time_unit = unit_str[0];
                        }
                        else
                        {
                            fprintf(stderr, L_TD_LINE_ERR_VALUE, entry -> d_name, line_count, key);
                            continue;
                        }
                        time_ = 1;
                    }
                    else if (strcmp(key, "date") == 0)
                    {
                        // 这里不做检查，如果为错值 atol 会返回 0
                        config[read_config].old_time = (time_t)atol(value);
                        date_ = 1;
                    }
                    else if (strcmp(key, "run") == 0)
                    {
                        // 不检查，其实也不好检查
                        snprintf(config[read_config].run, sizeof(config[read_config].run), "%s", value);
                        run_ = 1;
                    }
                    else if (strcmp(key, "post") == 0)
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
                    continue; // 跳过，read_config不自增，后面解析配置覆盖
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
    int fd = open("/dev/null", O_RDWR);
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    close(fd);
    
    // Post
    char start_success_str[256] = {0}; // 对应宏内容不能超过此大小
    snprintf(start_success_str, sizeof(start_success_str), L_TD_START_SUCCESS, read_config);
    post(SERVER_NAME, start_success_str);
    
    for ( ; ; ) // 主循环
    {
        struct tm now_time;
        time_t time_tm = time(NULL);
        localtime_r(&time_tm, &now_time);
        
        
        int i = 0;
        while (i < read_config)
        {
            struct tm old_time;
            localtime_r(&config[i].old_time, &old_time);
            
            int run = 0;
            switch (config[i].time_unit) // 匹配配置对应时间单位判断是否需要执行
            {
                case 'D':
                    // day
                    if ((now_time.tm_mday - old_time.tm_mday) >= config[i].time_num ||
                       now_time.tm_mon != old_time.tm_mon ||
                       now_time.tm_year != old_time.tm_year)
                    {
                        // 在0点到5点之间执行
                        if (now_time.tm_hour >= 0 &&
                           now_time.tm_hour <= 5)
                        {
                            run = 1;
                        }
                    }
                    break;
                case 'H':
                    // hour
                    if ((now_time.tm_hour - old_time.tm_hour) >= config[i].time_num ||
                       now_time.tm_mday != old_time.tm_mday)
                    {
                        run = 1;
                    }
                    break;
                case 'M':
                    // minute
                    if ((now_time.tm_min - old_time.tm_min) >= config[i].time_num ||
                       now_time.tm_hour != old_time.tm_hour)
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
                
                config[i].old_time = time_tm;
                char config_file[strlen(argv[1]) + strlen(config[i].config_name) + 2];
                snprintf(config_file, sizeof(config_file), "%s/%s", argv[1], config[i].config_name);
                
                /* 如果配置存在则更新，否则不重新创建，这是为了配合前端
                删掉配置即停用的设置。重启进程前配置仍然生效*/
                if (access(config_file, F_OK) == 0)
                {
                    FILE * config_file_fp = fopen(config_file, "w");
                    if (config_file_fp)
                    {
                        if (config[i].post == 1)
                        {
                            fprintf(config_file_fp, "time=%d/%c\ndate=%ld\nrun=%s\npost=%s/%s",
                                    // time
                                    config[i].time_num, config[i].time_unit,
                                    // date
                                    config[i].old_time,
                                    // run
                                    config[i].run,
                                    // post
                                    config[i].title, config[i].message
                                  );
                        }
                        else
                        {
                            fprintf(config_file_fp, "time=%d/%c\ndate=%ld\nrun=%s",
                                    // time
                                    config[i].time_num, config[i].time_unit,
                                    // date
                                    config[i].old_time,
                                    // run
                                    config[i].run
                                  );
                        }
                        fclose(config_file_fp);
                    }
                }
            }
            i++;
        }
        
        // 回收子进程
        while (waitpid(-1, NULL, WNOHANG) > 0);
        sleep(WAIT_TIME);
    }
}

static int running(char * command)
{
    // 解析命令参数
    char command_cope[MAX_COMMAND_LEN] = {0};
    snprintf(command_cope, sizeof(command_cope), "%s", command);
    
    int count = 0;
    char * arg = NULL;
    char * args[MAX_COMMAND_ARGS] = {NULL};
    arg = strtok(command_cope, " ");
    while (arg != NULL && count < (MAX_COMMAND_ARGS - 1))
    {
        args[count] = arg;
        count++;
        arg = strtok(NULL, " ");
    }
    if (count == 0)
    {
        return 1;
    }
    args[MAX_COMMAND_ARGS - 1] = NULL;
    
    // 执行命令
    pid_t new_pid = fork();
    if (new_pid == -1)
    {
        return 1;
    }
    if (new_pid == 0)
    {
        execvp(args[0], args);
        _exit(errno);
    }
    return 0;
}
