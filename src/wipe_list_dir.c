// 此脚本来自ClearBox模块，用于自定义规则清理/干掉文件目录
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/wait.h>
#include <string.h>

#define CONFIG_DIR_NAME "清理配置"

static int remove_all(char * path);

int main(int COMI, char * COM[])
{
    if (getuid() != 0)
    {
        printf(" » 请授予root权限！\n");
        return 1;
    }
    if (COMI < 2)
    {
        printf(" » 参数不足！\n");
        return 1;
    }
    
    char work_dir[64] = "";
    for (int i = 0; i < COMI - 1; i++)
    {
        if (strcmp(COM[i], "-w") == 0)
        {
            if (access(COM[i + 1], F_OK) != 0)
            {
                printf(" » 配置路径不可访问！\n");
                return 1;
            }
            snprintf(work_dir, sizeof(work_dir), "%s", COM[i + 1]);
            work_dir[strcspn(work_dir, "\n")] = 0;
        }
    }
    if (strcmp(work_dir, "") == 0)
    {
        printf(" » 配置路径参数为空！\n");
        return 1;
    }
    
    char config_dir[strlen(work_dir) + strlen(CONFIG_DIR_NAME) + 2];
    config_dir[0] = '\0';
    snprintf(config_dir, sizeof(config_dir), "%s/%s", work_dir, CONFIG_DIR_NAME);
    
    struct dirent * config_file_name;
    DIR * config_dir_dp = opendir(config_dir);
    if (config_dir_dp == NULL)
    {
        printf(" » 配置目录打开失败！\n");
        return 1;
    }
    
    while ((config_file_name = readdir(config_dir_dp)))
    {
        if (strcmp(config_file_name -> d_name, ".") == 0 ||
           strcmp(config_file_name -> d_name, "..") == 0)
        {
            continue;
        }
        
        char config_file[strlen(config_file_name -> d_name) + strlen(config_dir) + 2];
        config_file[0] = '\0';
        snprintf(config_file, sizeof(config_file), "%s/%s", config_dir, config_file_name -> d_name);
        
        FILE * config_file_fp = fopen(config_file, "r");
        if (config_file_fp == NULL)
        {
            printf(" » %s 配置打开失败！自动跳过\n", config_file_name -> d_name);
            continue;
        }
        
        printf(" » 处理 %s 配置📍\n", config_file_name -> d_name);
        
        int count = 0;
        char config_file_line[256] = "", dir[256] = "";
        while (fgets(config_file_line, sizeof(config_file_line), config_file_fp))
        {
            config_file_line[strcspn(config_file_line, "\n")] = 0;
            
            count++;
            char * config_file_line_ptr = config_file_line;
            while (isspace(* config_file_line_ptr)) config_file_line_ptr++;
            
            // 如果该行被注释则返回
            if (* config_file_line_ptr == '#')
            {
                continue;
            }
            
            if (* config_file_line_ptr == '@')
            {
                //初始目录
                char * key_str = strtok(config_file_line, "@");
                key_str = strtok(config_file_line, "@");
                if (key_str)
                {
                    snprintf(dir, sizeof(dir), "%s", key_str);
                    if (access(dir, F_OK) != 0)
                    {
                        printf(" » %s 配置指定初始路径不存在！\n", config_file_name -> d_name);
                    }
                }
                continue;
            }
            
            if (* config_file_line_ptr == '/')
            {
                if (access(config_file_line, F_OK))
                {
                    if (remove_all(config_file_line) == 0)
                    {
                        printf(" » 清理 %s 成功\n", config_file_line);
                    }
                    else
                    {
                        printf(" » 清理 %s 失败\n", config_file_line);
                    }
                }
                else
                {
                    printf(" » %d 行错误：路径不存在\n", count);
                    continue;
                }
            }
            else
            {
                char path[strlen(config_file_line) + strlen(dir) + 16];
                path[0] = '\0';
                snprintf(path, sizeof(path), "%s/%s", dir, config_file_line);
                if (access(path, F_OK) != 0)
                {
                    printf(" » %d 行错误：路径错误或不存在\n", count);
                    continue;
                }
                if (remove_all(path) == 0)
                {
                    printf(" » 清理 %s 成功\n", path);
                }
                else
                {
                    printf(" » 清理 %s 失败\n", path);
                }
            }
        }
        fclose(config_file_fp);
    }
    closedir(config_dir_dp);
    
    printf(" » 自定义目录处理完成！\n");
    return 0;
}

static int remove_all(char * path)
{
    if (access(path, F_OK) != 0)
    {
        return 1;
    }
    
    pid_t newPid = fork();
    if (newPid == -1)
    {
        return 1;
    }
    if (newPid == 0)
    {
        execlp("rm", "rm", "-rf", path, NULL);
        _exit(127);
    }
    else
    {
        int end = 0;
        if (waitpid(newPid, &end, 0) == -1)
        {
            return 1;
        }
        if (WIFEXITED(end) && WEXITSTATUS(end) != 0)
        {
            return 1;
        }
    }
    return 0;
}
