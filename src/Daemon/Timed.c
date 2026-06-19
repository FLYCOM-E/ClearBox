/*
                    GNU GENERAL PUBLIC
                        Version 3

 此 Code 来自 ClearBox 模块，用于自动任务配置解析与执行（DAEMON）
*/

#include "../INCLUDE/BashCore.h"

#define SERVER_NAME "ClearBox Timed"   // 进程名
#define WAIT_TIME 60                   // 单次循环等待秒数（自动对齐）
#define MAX_CONFIG 512                 // 最大配置数量
#define MAX_CONFIG_NAME 256            // 配置文件名称最大长度
#define CONFIG_LINE_MAX_LEN 512        // 配置文件行最大长度
#define MAX_COMMAND_LEN 4096           // 命令最大长度
#define MAX_TITLE_LEN 128              // 通知标题最大长度
#define MAX_MESSAGE_LEN 512            // 通知内容最大长度
#define CONFIG_PATH_NAME "TimedConfigs"// 配置目录名

struct config_struct
{
    // For time
    char time_unit;
    long time_num;
    // For date
    time_t old_time;
    // For post
    int post;
    char title[MAX_TITLE_LEN];
    char message[MAX_MESSAGE_LEN];
    // For in time
    int in;
    long start_hour;
    long end_hour;
    // Other
    char run[MAX_COMMAND_LEN];
    char config_name[MAX_CONFIG_NAME];
    time_t last_error_notify;
    int enable;
};

static int read_config(char * config_dir, volatile int * read_config_count, struct config_struct config[], char * config_file_name);
static int get_config(char * config_file, char * config_file_name, struct config_struct config[], int count);
static int running(char * command);

int time_daemon(char * argv[], char * work_dir)
{
    // 设置命名空间
    if (set_name_space() != 0)
    {
        return -1;
    }
    
    // 拼接及检查配置目录
    char config_dir[strlen(work_dir) + sizeof(CONFIG_PATH_NAME) + 2];
    snprintf(config_dir, sizeof(config_dir), "%s/%s", work_dir, CONFIG_PATH_NAME);
    
    volatile int read_config_count = 0;
    struct config_struct config[MAX_CONFIG] = {0}; // 创建结构体
    if (read_config(config_dir, &read_config_count, config, "") != 0)
    {
        // 错误信息由 read_config 函数报告
        return -1;
    }
    if (read_config_count == 0)
    {
        fprintf(stderr, L_NOCONFIG);
        return -1;
    }
    
    // INOTIFY
    int inotify_fd = inotify_init1(IN_NONBLOCK);
    if (inotify_fd == -1)
    {
        fprintf(stderr, L_TD_WATCH_CONFIG_DIR_ERROR, strerror(errno));
        return -1;
    }
    int inotify_wd = inotify_add_watch(inotify_fd, config_dir, IN_CLOSE_WRITE | IN_CREATE | IN_DELETE_SELF);
    if (inotify_wd == -1)
    {
        fprintf(stderr, L_TD_WATCH_CONFIG_DIR_ERROR, strerror(errno));
        return -1;
    }
    int watch = 1;
    char inotify_buffer[PATH_MAX] = "";
    
    // Daemon
    char log_text[strlen(L_SERVER_START_ERR) + strlen(L_TD_START_SUCCESS) + 128];
    if (s_daemon() != 0)
    {
        snprintf(log_text, sizeof(log_text), L_SERVER_START_ERR, strerror(errno));
        write_log(work_dir, SERVER_NAME, log_text);
        return -1;
    }
    if (s_signed() != 0)
    {
        snprintf(log_text, sizeof(log_text), L_SERVER_START_ERR, strerror(errno));
        write_log(work_dir, SERVER_NAME, log_text);
        return -1;
    }
    else
    {
        sig_flag = 1;
    }
    snprintf(log_text, sizeof(log_text), L_TD_START_SUCCESS, read_config_count, getpid());
    post(SERVER_NAME, log_text, SERVER_NAME);
    write_log(work_dir, SERVER_NAME, log_text);
    set_server_name(argv, SERVER_NAME);
    
    while (sig_flag)
    {
        if (watch == 1)
        {
            ssize_t inotify_len = read(inotify_fd, inotify_buffer, sizeof(inotify_buffer));
            if (inotify_len != -1)
            {
                char * inotify_buffer_p = inotify_buffer;
                char * inotify_len_p = inotify_buffer + inotify_len;
                
                while (inotify_buffer_p < inotify_len_p)
                {
                    int file_edited = 0;
                    struct inotify_event * event = (struct inotify_event *)inotify_buffer_p;
                    if (event -> mask & IN_DELETE_SELF)
                    {
                        inotify_rm_watch(inotify_fd, (uint32_t)inotify_wd);
                        watch = 0;
                        break;
                    }
                    if (event -> mask & IN_CLOSE_WRITE)
                    {
                        file_edited = 1;
                    }
                    if (event -> mask & IN_CREATE)
                    {
                        file_edited = 1;
                    }
                    
                    if (file_edited == 1 && (!(event -> mask & IN_ISDIR)))
                    {
                        if (read_config(config_dir, &read_config_count, config, event -> name) == 0)
                        {
                            char message[strlen(L_TD_LOAD_CONFIG_SUCCESS) + strlen(event -> name) + 2];
                            snprintf(message, sizeof(message), L_TD_LOAD_CONFIG_SUCCESS, event -> name);
                            post(SERVER_NAME, message, event -> name);
                        }
                    }
                    
                    inotify_buffer_p += sizeof(struct inotify_event) + event -> len;
                }
            }
        }
        else
        {
            if (access(config_dir, F_OK) == 0)
            {
                inotify_wd = inotify_add_watch(inotify_fd, config_dir, IN_CLOSE_WRITE | IN_CREATE | IN_DELETE_SELF);
                watch = 1;
                continue;
            }
        }
        
        time_t now_time = time(NULL);
        struct tm now_time_local;
        localtime_r(&now_time, &now_time_local);
        
        int i = 0;
        while (i < read_config_count)
        {
            // 检查停用状态
            if (config[i].enable != 1)
            {
                i++;
                continue;
            }
            
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
                    if (difftime(now_time, config[i].old_time) >= (double)(config[i].time_num * 86400))
                    {
                        run = 1;
                    }
                    break;
                case 'H':
                    // hour
                    if (difftime(now_time, config[i].old_time) >= (double)(config[i].time_num * 3600))
                    {
                        run = 1;
                    }
                    break;
                case 'M':
                    // minute
                    if (difftime(now_time, config[i].old_time) >= (double)(config[i].time_num * 60))
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
                    post(config[i].title, config[i].message, config[i].config_name);
                }
                
                // 配置文件
                char config_file[strlen(config_dir) + strlen(config[i].config_name) + 2];
                snprintf(config_file, sizeof(config_file), "%s/%s", config_dir, config[i].config_name);
                
                /* 
                回写
                如果配置存在则更新，否则设置为停用。
                这是为了配合前端删掉配置即停用的设置。
                */
                if (access(config_file, F_OK) == 0)
                {
                    char line[128] = "";
                    snprintf(line, sizeof(line), "date=%ld", now_time);
                    
                    // 更新 DATE
                    inotify_rm_watch(inotify_fd, (uint32_t)inotify_wd);
                    int success = s_sed(config_file, "date=", line, 1);
                    inotify_wd = inotify_add_watch(inotify_fd, config_dir, IN_CLOSE_WRITE | IN_CREATE | IN_DELETE_SELF);
                    
                    // Check Errno
                    if (success != 0 &&
                        difftime(now_time, config[i].last_error_notify) >= 3600) // 1小时
                    {
                        char error_text[512] = {0};
                        snprintf(error_text, sizeof(error_text), L_TD_CONFIG_WRITE_ERROR,
                                 config[i].config_name);
                        post(SERVER_NAME, error_text, SERVER_NAME);
                        config[i].last_error_notify = now_time; // 记录上次通知时间，避免短时间多次通知
                    }
                }
                else
                {
                    config[i].enable = 0;
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
    
    close(inotify_fd);
    return 0;
}

/*
加载配置
接收：
    char * config_dir 配置目录
    int * read_config_count 已读取配置数量
    struct config_struct config[] 结构体数组
    char * config_file_name 指定要读取的配置名称，为空则全量读取
返回：
    成功返回 0，失败返回 -1
*/
static int read_config(char * config_dir, volatile int * read_config_count, struct config_struct config[], char * config_file_name)
{
    if (strcmp(config_file_name, "") != 0)
    {
        // 热重载场景处理，搜索可用槽位，优先寻找已解析槽位
        int index = -1, last_index = -1;
        for (int i = 0; i < MAX_CONFIG; i++)
        {
            if (strcmp(config[i].config_name, config_file_name) == 0)
            {
                index = i;
                break;
            }
            if (config[i].enable == 0 && last_index == -1)
            {
                last_index = i;
            }
            if (i > * read_config_count && last_index != -1)
            {
                break;
            }
        }
        if (index == -1)
        {
            if (last_index == -1)
            {
                printf(L_TD_MAX_CONFIG, MAX_CONFIG);
                return -1;
            }
            else
            {
                index = last_index;
            }
        }
        
        char config_file[strlen(config_dir) + strlen(config_file_name) + 2];
        snprintf(config_file, sizeof(config_file), "%s/%s", config_dir, config_file_name);
        if (get_config(config_file, config_file_name, config, index) != 0)
        {
            return -1;
        }
        
        // 设置配置为启用并保留名称
        config[index].enable = 1;
        snprintf(config[index].config_name, sizeof(config[index].config_name), "%s", config_file_name);
        printf(L_TD_CONFIG_SUCCESS, config_file_name);
        
        if (index >= * read_config_count)
        {
            * read_config_count += 1;
        }
        return 0;
    }
    
    struct dirent * entry;
    DIR * config_dir_dp = opendir(config_dir);
    if (config_dir_dp == NULL)
    {
        fprintf(stderr, L_OPEN_PATH_FAILED, config_dir, strerror(errno));
        return -1;
    }
    // 遍历配置目录
    while ((entry = readdir(config_dir_dp)))
    {
        if (strcmp(entry -> d_name, ".") == 0 ||
           strcmp(entry -> d_name, "..") == 0)
        {
            continue;
        }
                
        // 热重载场景处理，搜索可用槽位，优先寻找已解析槽位
        int index = -1, last_index = -1;
        for (int i = 0; i < MAX_CONFIG; i++)
        {
            if (strcmp(config[i].config_name, entry -> d_name) == 0)
            {
                index = i;
                break;
            }
            if (config[i].enable == 0 && last_index == -1)
            {
                last_index = i;
            }
            if (i > * read_config_count && last_index != -1)
            {
                break;
            }
        }
        if (index == -1)
        {
            if (last_index == -1)
            {
                printf(L_TD_MAX_CONFIG, MAX_CONFIG);
                return -1;
            }
            else
            {
                index = last_index;
            }
        }
                
        char config_file[strlen(config_dir) + strlen(entry -> d_name) + 2];
        snprintf(config_file, sizeof(config_file), "%s/%s", config_dir, entry -> d_name);
        if (get_config(config_file, entry -> d_name, config, index) != 0)
        {
            continue;
        }
        
        // 设置配置为启用并保留名称
        config[index].enable = 1;
        snprintf(config[index].config_name, sizeof(config[index].config_name), "%s", entry -> d_name);
        printf(L_TD_CONFIG_SUCCESS, entry -> d_name);
        
        if (index >= * read_config_count)
        {
            * read_config_count += 1;
        }
        
        if (* read_config_count == MAX_CONFIG) // 限制配置数量
        {
            printf(L_TD_MAX_CONFIG, MAX_CONFIG);
            break;
        }
    }
    closedir(config_dir_dp);
    
    return 0;
}

/*
配置解析
接收：
    char * config_file 配置文件
    char * config_file_name 配置文件名
    struct config_struct config[] 读入结构体数组
    int count 数组写入位置
返回：
    成功返回 0，失败返回 -1
*/
static int get_config(char * config_file, char * config_file_name, struct config_struct config[], int count)
{
    int line_count = 0; // 行计数
    int time_ = 0, date_ = 0, run_ = 0; // 记录是否解析标志位
    char line[CONFIG_LINE_MAX_LEN] = {0};
    
    // 热更新 & 重置部分字段
    config[count].post = 0;
    config[count].in = 0;
    
    // 非普通文件即失败，同时提示这里有非文件
    FILE * config_fp = fopen(config_file, "r");
    if (config_fp == NULL)
    {
        fprintf(stderr, L_OPEN_FILE_FAILED, config_file_name, strerror(errno));
        return -1;
    }
    while (fgets(line, sizeof(line), config_fp))
    {
        line_count++;
        line[strcspn(line, "\n")] = 0;
        // 注释跳过
        if (line[0] == '#')
        {
            continue;
        }
        
        // 解析 KEY/VALUE
        char * line_p = NULL;
        char * key = strtok_r(line, "=", &line_p);
        char * value = strtok_r(NULL, "=", &line_p);
        if (value == NULL || key == NULL)
        {
            fprintf(stderr, L_TD_LINE_ERR_VALUE, config_file_name, line_count, key);
            continue;
        }
        
        // 匹配提取键值读入结构体
        if (strcasecmp(key, "time") == 0)
        {
            char * value_p = NULL;
            char * time_str = strtok_r(value, "/", &value_p);
            char * unit_str = strtok_r(NULL, "/", &value_p);
            if (time_str && unit_str &&
                strtol(time_str, NULL, 10) != 0 &&
                (unit_str[0] == 'D' ||
                unit_str[0] == 'H' ||
                unit_str[0] == 'M'))
            {
                config[count].time_unit = unit_str[0];
                config[count].time_num = strtol(time_str, NULL, 10);
                time_ = 1; // 已读取 TIME
            }
            else
            {
                fprintf(stderr, L_TD_LINE_ERR_VALUE, config_file_name, line_count, key);
                continue;
            }
        }
        else if (strcasecmp(key, "date") == 0)
        {
            // 这里不做检查，如空则设置为当前时间
            config[count].old_time = (time_t)strtol(value, NULL, 10);
            if (config[count].old_time <= 0)
            {
                if (config[count].old_time == -1)
                {
                    config[count].old_time = 0;
                }
                else
                {
                    config[count].old_time = time(NULL);
                }
            }
            date_ = 1; // 已读取 DATE
        }
        else if (strcasecmp(key, "run") == 0)
        {
            // 不检查命令（不好做检查）
            if (strlen(value) >= MAX_COMMAND_LEN)
            {
                fprintf(stderr, L_TD_LINE_ERR_VALUE, config_file_name, line_count, key);
                continue;
            }
            snprintf(config[count].run, sizeof(config[count].run), "%s", value);
            run_ = 1;
        }
        else if (strcasecmp(key, "in") == 0)
        {
            config[count].start_hour = 0;
            config[count].end_hour = 0;
            
            char * value_p = NULL;
            char * start_tm_str = strtok_r(value, "/", &value_p);
            char * end_tm_str = strtok_r(NULL, "/", &value_p);
            if (start_tm_str && end_tm_str)
            {
                long start_hour = 0, end_hour = 0;
                start_hour = strtol(start_tm_str, NULL, 10);
                end_hour = strtol(end_tm_str, NULL, 10);
                if (start_hour > 23 ||
                    start_hour < 0 ||
                    end_hour > 23 ||
                    end_hour < 0)
                {
                    fprintf(stderr, L_TD_LINE_ERR_VALUE, config_file_name, line_count, key);
                    continue;
                }
                
                config[count].start_hour = start_hour;
                config[count].end_hour = end_hour;
                config[count].in = 1;
            }
            else
            {
                fprintf(stderr, L_TD_LINE_ERR_VALUE, config_file_name, line_count, key);
                continue;
            }
        }
        else if (strcasecmp(key, "post") == 0)
        {
            char * value_p = NULL;
            char * message = strchr(value, '/');
            char * title = strtok_r(value, "/", &value_p);
            if (title && message)
            {
                if (strlen(title) > MAX_TITLE_LEN)
                {
                    fprintf(stderr, L_TD_W_POST_TITLE_TOOLONG, config_file_name);
                }
                if (strlen(message + 1) > MAX_MESSAGE_LEN)
                {
                    fprintf(stderr, L_TD_W_POST_MESSAGE_TOOLONG, config_file_name);
                }
                snprintf(config[count].title, sizeof(config[count].title), "%s", title);
                snprintf(config[count].message, sizeof(config[count].message), "%s", message + 1);
                config[count].post = 1;
            }
            else
            {
                fprintf(stderr, L_TD_LINE_ERR_VALUE, config_file_name, line_count, key);
                continue;
            }
        }
        else
        {
            fprintf(stderr, L_TD_LINE_ERR_KEY, config_file_name, line_count, key);
        }
    }
    fclose(config_fp);
    
    if (time_ != 1 ||
        date_ != 1 ||
        run_ != 1) // post、in 非必须字段不检查
    {
        fprintf(stderr, L_TD_CONFIG_ERROR, config_file_name);
        return -1;
    }
    
    return 0;
}

/* 运行命令，不等待子进程（由父进程回收） */
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
