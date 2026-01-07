// 此 Code 来自ClearBox模块，是crond代替品哦～
#include "INCLUDE/BashCore.h"

#define WAIT_TIME 60
#define MAX_CONFIG 64
#define MAX_COMMAND_LEN 512
#define SERVER_NAME "ClearBox Timed"

static int Running(char * command);

struct config_file
{
    // for time
    char time_unit;
    int time_num;
    // for date
    int date_year;
    int date_month;
    int date_day;
    int date_hour;
    int date_minute;
    // other
    char run[MAX_COMMAND_LEN];
    char config_name[128];
};

int main(int argc, char * argv[])
{
    if (getuid() != 0)
    {
        printf(" » Please use root privileges!\n");
        return 1;
    }
    if (argc != 2)
    {
        printf("E Args error\n");
        return 1;
    }
    if (access(argv[1], F_OK) != 0)
    {
        printf("E %s no such file or dir\n", argv[1]);
        return 1;
    }
    
    int read_config = 0;
    struct config_file config[MAX_CONFIG];
    
    struct stat path_stat;
    if (lstat(argv[1], &path_stat) == -1)
    {
        printf("E stat %s error\n", argv[1]);
        return 1;
    }
    
    if (S_ISDIR(path_stat.st_mode))
    {
        struct stat file_stat;
        struct dirent * entry;
        DIR * config_dir_dp = opendir(argv[1]);
        if (config_dir_dp == NULL)
        {
            printf("E Open dir %s error\n", argv[1]);
            return 1;
        }
        while ((entry = readdir(config_dir_dp)))
        {
            if (strcmp(entry -> d_name, ".") == 0 ||
               strcmp(entry -> d_name, "..") == 0)
            {
                continue;
            }
            
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
                int line_count = 0;
                char line[128] = {0};
                FILE * config_fp = fopen(path, "r");
                if (config_fp == NULL)
                {
                    printf("W %s open error\n", entry -> d_name);
                    continue;
                }
                while (fgets(line, sizeof(line), config_fp))
                {
                    line_count++;
                    line[strcspn(line, "\n")] = 0;
                    char * key = strtok(line, "=");
                    char * value = strtok(NULL, "=");
                    if (value == NULL)
                    {
                        printf("E %s: line %d %s= error. Skip\n", entry -> d_name, line_count, key);
                        break;
                    }
                
                    if (strcmp(key, "time") == 0)
                    {
                        config[read_config].time_num = atoi(strtok(value, "/"));
                        char * unit_str = strtok(NULL, "/");
                        config[read_config].time_unit = unit_str[0];
                    }
                    else if (strcmp(key, "date") == 0)
                    {
                        config[read_config].date_year = atoi(strtok(value, "/"));
                        config[read_config].date_month = atoi(strtok(NULL, "/"));
                        config[read_config].date_day = atoi(strtok(NULL, "/"));
                        config[read_config].date_hour = atoi(strtok(NULL, "/"));
                        config[read_config].date_minute = atoi(strtok(NULL, "/"));
                    }
                    else if (strcmp(key, "run") == 0)
                    {
                        snprintf(config[read_config].run, sizeof(config[read_config].run), "%s", value);
                    }
                    else
                    {
                        printf("E %s: line %d error key: %s\n", entry -> d_name, line_count, key);
                    }
                }
                fclose(config_fp);
                snprintf(config[read_config].config_name, sizeof(config[read_config].config_name), "%s", entry -> d_name);
                read_config += 1;
                printf("I %s config is %d Success\n", entry -> d_name, read_config);
            }
        }
        closedir(config_dir_dp);
        
        if (read_config == 0)
        {
            printf("E Not any config files! \n");
            return 1;
        }
    }
    else
    {
        printf("E %s not is dir.\n", argv[1]);
        return 1;
    }
    
    pid_t pid = fork();
    if (pid == -1)
    {
        printf("Server start error\n");
        return 1;
    }
    if (pid != 0)
    {
        exit(0);
    }
    setsid();
    int fd = open("/dev/null", O_RDWR);
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    close(fd);
    post(SERVER_NAME, "Start Server Successful");
    
    for ( ; ; )
    {
        time_t now_time;
        struct tm * timeinfo;
        time(&now_time);
        timeinfo = localtime(&now_time);
        
        int current_year = timeinfo->tm_year + 1900;
        int current_month = timeinfo->tm_mon + 1;
        int current_day = timeinfo->tm_mday;
        int current_hour = timeinfo->tm_hour;
        int current_minute = timeinfo->tm_min;
        
        int i = 0;
        while (i < read_config)
        {
            int total_time = 0;
            switch (config[i].time_unit)
            {
                case 'D':
                    // total_time = day
                    total_time = (current_year - config[i].date_year) * 365 +
                                (current_month - config[i].date_month) * 30 +
                                (current_day - config[i].date_day);
                    break;
                case 'H':
                    // total_time = hour
                    total_time = (current_year - config[i].date_year) * 365 * 24 +
                                (current_month - config[i].date_month) * 30 * 24 +
                                (current_day - config[i].date_day) * 24 +
                                (current_hour - config[i].date_hour);
                    break;
                case 'M':
                    // total_time = minute
                    total_time = (current_year - config[i].date_year) * 365 * 24 * 60 +
                                (current_month - config[i].date_month) * 30 * 24 * 60 +
                                (current_day - config[i].date_day) * 24 * 60 +
                                (current_hour - config[i].date_hour) * 60 +
                                (current_minute - config[i].date_minute);
                    break;
                default:
                    printf("W %s time unit is error\n", config[i].config_name);
                    break;
            }
            
            if (total_time >= config[i].time_num)
            {
                Running(config[i].run);
                
                config[i].date_year = current_year;
                config[i].date_month = current_month;
                config[i].date_day = current_day;
                config[i].date_hour = current_hour;
                config[i].date_minute = current_minute;
                
                char config_file[strlen(argv[1]) + strlen(config[i].config_name) + 2];
                snprintf(config_file, sizeof(config_file), "%s/%s", argv[1], config[i].config_name);
                
                FILE * config_file_fp = fopen(config_file, "w");
                if (config_file_fp)
                {
                    fprintf(config_file_fp, "time=%d/%c\ndate=%d/%d/%d/%d/%d\nrun=%s",
                            // time
                            config[i].time_num, config[i].time_unit,
                            // date
                            config[i].date_year, config[i].date_month,
                            config[i].date_day, config[i].date_hour, 
                            config[i].date_minute,
                            // run
                            config[i].run
                          );
                    fclose(config_file_fp);
                }
            }
            i++;
        }
        sleep(WAIT_TIME);
    }
}

static int Running(char * command)
{
    char command_cope[MAX_COMMAND_LEN] = {0};
    snprintf(command_cope, sizeof(command_cope), "%s", command);
    
    int count = 0;
    char * arg = NULL;
    char * args[17] = {NULL};
    
    arg = strtok(command_cope, " ");
    while (arg != NULL && count < 16)
    {
        args[count] = arg;
        count++;
        arg = strtok(NULL, " ");
    }
    if (count == 0)
    {
        return 1;
    }
    args[16] = NULL;
    
    pid_t newPid = fork();
    if (newPid == -1)
    {
        return 1;
    }
    if (newPid == 0)
    {
        execvp(args[0], args);
        _exit(errno);
    }
    else
    {
        int end = 0;
        if (waitpid(newPid, &end, 0) == -1)
        {
            return end;
        }
        if (WIFEXITED(end) && WEXITSTATUS(end) != 0)
        {
            return end;
        }
    }
    
    return 0;
}
