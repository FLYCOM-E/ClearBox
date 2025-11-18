// 此脚本来自ClearBox模块，用于自定义规则软件内部清理
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_PACKAGE 128

int findPackageInProFile(char * package, char * ProFile);

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
    char AppPackage[strlen(COM[1]) + 16];
    AppPackage[0] = '\0';
    snprintf(AppPackage, sizeof(AppPackage), "%s", COM[1]);
    
    // data dir
    char dataDir[64] = "/data/data";
    
    // work dir
    char work_dir[64] = "";
    FILE * work_dir_fp = popen("ClearBox -w", "r");
    fgets(work_dir, sizeof(work_dir), work_dir_fp);
    pclose(work_dir_fp);
    work_dir[strcspn(work_dir, "\n")] = 0;
    
    // config dir
    char configDir[strlen(work_dir) + 16];
    configDir[0] = '\0';
    snprintf(configDir, sizeof(configDir), "%s/清理规则", work_dir);
    if (access(configDir, F_OK) != 0)
    {
        printf(" » 无App清理配置！\n");
        return 1;
    }
    
    // 遍历配置目录 
    int getConfig = 0;
    struct dirent * configName;
    DIR * configDir_fp = opendir(configDir);
    if (configDir_fp == NULL)
    {
        printf(" » %s 目录打开失败！\n", configDir);
        return 1;
    }
    
    while ((configName = readdir(configDir_fp)))
    {
        if (strcmp(configName -> d_name, ".") == 0 ||
           strcmp(configName -> d_name, "..") == 0)
        {
            continue;
        }
        
        //configFile: 配置文件
        char configFile[strlen(configDir) + strlen(configName -> d_name) + 2];
        configFile[0] = '\0';
        snprintf(configFile, sizeof(configFile), "%s/%s", configDir, configName -> d_name);
        
        // 这个函数目标是检查配置文件是否为目标配置文件
        if (findPackageInProFile(AppPackage, configFile) == 1)
        {
            FILE * config_fp = fopen(configFile, "r");
            if (config_fp == NULL)
            {
                printf(" » 警告：配置打开失败！非预期行为！\n");
                closedir(configDir_fp);
                continue;
                // 这里本来应直接退出，这是异常行为
                // 因为 findPackageInProFile 函数不应返回 1
            }
            
            // 遍历配置文件
            int count = 0;
            char dir[strlen(dataDir) + 64];
            dir[0] = '\0'; 
            char str[256] = "", Package[64] = "", Name[64] = "";
            char * Package_fp = NULL, * Name_fp = NULL;
            
            while (fgets(str, sizeof(str), config_fp))
            {
                str[strcspn(str, "\n")] = 0;
                count++;
                
                /*
                配置文件第一行以＠开头
                格式：
                @<软件包名>/<软件名称>
                */
                if (strstr(str, "@"))
                {
                    Package_fp = strtok(str, "/");
                    Name_fp = strtok(NULL, "/");
                    if (Package_fp && Name_fp)
                    {
                        snprintf(Package, sizeof(Package), "%s", Package_fp + 1); //软件包名
                        snprintf(Name, sizeof(Name), "%s", Name_fp);        //软件名称
                        snprintf(dir, sizeof(dir), "%s/%s", dataDir, Package);   //软件目录
                        
                        if (access(dir, F_OK) == 0)
                        {
                            printf(" » 清理 %s &\n", Name);
                            getConfig = 1;
                            continue;
                        }
                        else
                        {
                            printf(" » %s：配置指定软件未找到！\n", configName -> d_name);
                            break;
                        }
                    }
                    else
                    {
                        printf(" » %s 配置声明错误\n", configName -> d_name);
                        break;
                    }
                }
                else
                {
                    // getConfig = 1: 已读取文件声明信息
                    if (getConfig == 1)
                    {
                        char appDir[strlen(dir) + strlen(str) + 2];
                        appDir[0] = '\0';
                        snprintf(appDir, sizeof(appDir), "%s/%s", dir, str);
                        
                        char * ptr = str;
                        // 如果该行被注释则返回
                        while(isspace(* ptr)) ptr++;
                        if (* ptr == '#')
                        {
                            continue;
                        }
                        // 配置不应以／开头！
                        if (* ptr == '/')
                        {
                            printf(" » %s 配置第 %d 行存在危险错误：从“／”开始！\n", configName -> d_name, count);
                            continue;
                        }
                        // 防止路径逃逸
                        if (strstr(str, "/../"))
                        {
                            printf(" » %s 配置第 %d 行存在错误：路径逃逸！", configName -> d_name, count);
                            continue;
                        }
                        // 这可以避免很多 rm 报错
                        if (access(appDir, F_OK) != 0)
                        {
                            continue;
                        }
                        
                        pid_t newPid = fork();
                        if (newPid == -1)
                        {
                            printf(" » 删除 %s 失败！(Fork)\n", str);
                            continue;
                        }
                        if (newPid == 0)
                        {
                            execlp("rm", "rm", "-r", appDir, NULL);
                            _exit(1);
                        }
                        else
                        {
                            int end = 0;
                            if (waitpid(newPid, &end, 0) == -1)
                            {
                                printf(" » 删除 %s 失败！(Wait)\n", str);
                                continue;
                            }
                            if (WIFEXITED(end) && WEXITSTATUS(end) != 0)
                            {
                                printf(" » 删除 %s 失败！\n", str);
                                continue;
                            }
                        }
                    }
                    else
                    {
                        printf(" » %s 配置错误！请在第一行正确填写声明\n", configName -> d_name);
                        break;
                    }
                }
            }
            
            fclose(config_fp);
        }
    }
    
    closedir(configDir_fp);
    
    if (getConfig == 1)
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
int findPackageInProFile(char * package, char * ProFile)
{
    char str[256] = "";
    if (access(ProFile, R_OK) != 0)
    {
        return -1;
    }
    FILE * ProFile_fp = fopen(ProFile, "r");
    if (ProFile_fp == NULL)
    {
        return -1;
    }
    while (fgets(str, sizeof(str), ProFile_fp))
    {
        str[strcspn(str, "\n")] = 0;
        
        if (strstr(str, package))
        {
            return 1;
        }
    }
    if (ProFile_fp) fclose(ProFile_fp);
    
    return 0;
}
