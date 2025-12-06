// 此脚本来自ClearBox模块，用于自定义规则软件内部清理
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_PACKAGE 256
#define DATA_DIR "/data/data"
#define CONFIG_DIR_NAME "清理规则"

static int findPackageInProFile(char * package, char * config_file);

int main(int COMI, char * COM[])
{
    if (getuid() != 0)
    {
        printf(" » 请授予root权限！\n");
        return 1;
    }
    if (COMI < 5)
    {
        printf(" » ERROR：未正确传递软件包名、配置文件目录！\n");
        return 1;
    }
    
    // Get work_dir & package
    char work_dir[64] = "", app_package[MAX_PACKAGE] = "";
    for (int i = 0; i < COMI - 1; i++)
    {
        if (strcmp(COM[i], "-w") == 0)
        {
            if (access(COM[i + 1], F_OK) != 0)
            {
                printf(" » 配置目录不存在！\n");
                return 1;
            }
            snprintf(work_dir, sizeof(work_dir), "%s", COM[i + 1]);
            work_dir[strcspn(work_dir, "\n")] = 0;
        }
        if (strcmp(COM[i], "-p") == 0)
        {
            if (strlen(COM[i + 1]) > MAX_PACKAGE)
            {
                printf(" » 传入包名过长！限制 %d\n", MAX_PACKAGE);
                return 1;
            }
            snprintf(app_package, sizeof(app_package), "%s", COM[i + 1]);
            app_package[strcspn(app_package, "\n")] = 0;
        }
    }
    if (strcmp(work_dir, "") == 0)
    {
        printf(" » 未传入配置目录！\n");
        return 1;
    }
    if (strcmp(app_package, "") == 0)
    {
        printf(" » 未传入包名！\n");
        return 1;
    }
    
    // config dir
    char config_dir[strlen(work_dir) + 16];
    config_dir[0] = '\0';
    snprintf(config_dir, sizeof(config_dir), "%s/%s", work_dir, CONFIG_DIR_NAME);
    if (access(config_dir, F_OK) != 0)
    {
        printf(" » 无App清理配置！\n");
        return 1;
    }
    
    // 遍历配置目录 
    int get_config = 0;
    struct dirent * config_name;
    DIR * config_dir_fp = opendir(config_dir);
    if (config_dir_fp == NULL)
    {
        printf(" » %s 目录打开失败！\n", config_dir);
        return 1;
    }
    
    while ((config_name = readdir(config_dir_fp)))
    {
        if (strcmp(config_name -> d_name, ".") == 0 ||
           strcmp(config_name -> d_name, "..") == 0)
        {
            continue;
        }
        
        //config_file: 配置文件
        char config_file[strlen(config_dir) + strlen(config_name -> d_name) + 2];
        config_file[0] = '\0';
        snprintf(config_file, sizeof(config_file), "%s/%s", config_dir, config_name -> d_name);
        
        // 这个函数目标是检查配置文件是否为目标配置文件
        if (findPackageInProFile(app_package, config_file) != 1)
        {
            continue;
        }
        
        FILE * config_fp = fopen(config_file, "r");
        if (config_fp == NULL)
        {
            printf(" » 警告：配置打开失败！非预期行为！\n");
            closedir(config_dir_fp);
            continue;
            // 这里本来应直接退出，这是异常行为
            // 因为 findPackageInProFile 函数不应返回 1
        }
        
        // 遍历配置文件
        int count = 0;
        char app_dir[64 + MAX_PACKAGE + 2];
        app_dir[0] = '\0'; 
        char len_str[256] = "", app_name[64] = "";
        char * app_package_fp = NULL, * app_name_fp = NULL;
        
        while (fgets(len_str, sizeof(len_str), config_fp))
        {
            len_str[strcspn(len_str, "\n")] = 0;
            count++;  //记录行数
            
            /*
            配置文件第一行以＠开头
            格式：
            @<软件包名>/<软件名称>
            */
            if (strstr(len_str, "@"))
            {
                app_package_fp = strtok(len_str, "/");
                app_name_fp = strtok(NULL, "/");
                if (app_package_fp && app_name_fp)
                {
                    snprintf(app_name, sizeof(app_name), "%s", app_name_fp);                  //软件名称
                    snprintf(app_dir, sizeof(app_dir), "%s/%s", DATA_DIR, app_package_fp + 1);   //软件目录
                    
                    if (access(app_dir, F_OK) == 0)
                    {
                        printf(" » 清理 %s &\n", app_name);
                        get_config = 1;
                        continue;
                    }
                    else
                    {
                        printf(" » %s：配置指定软件未找到！\n", config_name -> d_name);
                        break;
                    }
                }
                else
                {
                    printf(" » %s 配置声明错误\n", config_name -> d_name);
                    break;
                }
            }
            else
            {
                // get_config = 1: 已读取文件声明信息
                if (get_config == 1)
                {
                    char app_cf_dir[strlen(app_dir) + strlen(len_str) + 2];
                    app_cf_dir[0] = '\0';
                    snprintf(app_cf_dir, sizeof(app_cf_dir), "%s/%s", app_dir, len_str);
                    
                    char * len_str_ptr = len_str;
                    // 如果该行被注释则返回
                    while(isspace(* len_str_ptr)) len_str_ptr++;
                    if (* len_str_ptr == '#')
                    {
                        continue;
                    }
                    // 配置不应以／开头！
                    if (* len_str_ptr == '/')
                    {
                        printf(" » %s 配置第 %d 行存在危险错误：从“／”开始！\n", config_name -> d_name, count);
                        continue;
                    }
                    // 防止路径逃逸
                    if (strstr(len_str, "/../"))
                    {
                        printf(" » %s 配置第 %d 行存在错误：路径逃逸！", config_name -> d_name, count);
                        continue;
                    }
                    // 这可以避免很多 rm 报错
                    if (access(app_cf_dir, F_OK) != 0)
                    {
                        continue;
                    }
                    
                    pid_t newPid = fork();
                    if (newPid == -1)
                    {
                        printf(" » 删除 %s 失败！(Fork)\n", len_str);
                        continue;
                    }
                    if (newPid == 0)
                    {
                        execlp("rm", "rm", "-r", app_cf_dir, NULL);
                        _exit(1);
                    }
                    else
                    {
                        int end = 0;
                        if (waitpid(newPid, &end, 0) == -1)
                        {
                            printf(" » 删除 %s 失败！(Wait)\n", len_str);
                            continue;
                        }
                        if (WIFEXITED(end) && WEXITSTATUS(end) != 0)
                        {
                            printf(" » 删除 %s 失败！\n", len_str);
                            continue;
                        }
                    }
                }
                else
                {
                    printf(" » %s 配置错误！请在第一行正确填写声明\n", config_name -> d_name);
                    break;
                }
            }
        }
        
        fclose(config_fp);
    }
    
    closedir(config_dir_fp);
    
    if (get_config == 1)
    {
        printf(" » 软件规则处理成功！\n");
    }
    else
    {
        printf(" » 未找到指定软件配置或发生错误！\n");
    }
}

/*
检查配置是否包含传入包名（预期配置）
出现异常返回-1，找到返回1，未找到返回0
*/
static int findPackageInProFile(char * package, char * config_file)
{
    int end = 0;
    char config_len[256] = "";
    if (access(config_file, R_OK) != 0)
    {
        end = -1;
    }
    FILE * config_file_fp = fopen(config_file, "r");
    if (config_file_fp == NULL)
    {
        end = -1;
    }
    while (fgets(config_len, sizeof(config_len), config_file_fp))
    {
        config_len[strcspn(config_len, "\n")] = 0;
        
        if (strstr(config_len, package))
        {
            end = 1;
        }
    }
    if (config_file_fp) fclose(config_file_fp);
    
    return end;
}
