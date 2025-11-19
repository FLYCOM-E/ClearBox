// 此脚本来自ClearBox模块，用于自定义规则软件内部清理
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_PACKAGE 128

int findPackageInProFile(char * package, char * config_file);

int main(int COMI, char * COM[])
{
    if (getuid() != 0)
    {
        printf(" » 请授予root权限！\n");
        return 1;
    }
    if (system("ClearBox -v >/dev/null 2>&1") != 0)
    {
        printf(" » 模块加载异常，请排查反馈！\n");
        return 1;
    }
    if (COMI < 2)
    {
        printf(" » ERROR：需要一个参数，未传入软件名称！\n");
        return 1;
    }
    
    // 接收传入包名
    if (strlen(COM[1]) > MAX_PACKAGE)
    {
        printf(" » 传入包名过长！长度 %zu，限制 %d\n", strlen(COM[1]), MAX_PACKAGE);
        return 1;
    }
    char app_package[MAX_PACKAGE] = "";
    snprintf(app_package, sizeof(app_package), "%s", COM[1]);
    
    // data dir
    char data_dir[64] = "/data/data";
    
    // work dir
    char work_dir[64] = "";
    FILE * work_dir_fp = popen("ClearBox -w", "r");
    fgets(work_dir, sizeof(work_dir), work_dir_fp);
    pclose(work_dir_fp);
    work_dir[strcspn(work_dir, "\n")] = 0;
    
    // config dir
    char config_dir[strlen(work_dir) + 16];
    config_dir[0] = '\0';
    snprintf(config_dir, sizeof(config_dir), "%s/清理规则", work_dir);
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
        char app_dir[strlen(data_dir) + 64];
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
                    snprintf(app_dir, sizeof(app_dir), "%s/%s", data_dir, app_package_fp + 1);   //软件目录
                    
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
int findPackageInProFile(char * package, char * config_file)
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
