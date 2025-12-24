// 此Code来自ClearBox模块，用于自定义规则软件内部清理
#include "BashCore.h"

#define MAX_APP_NAME 128
#define DATA_DIR "/data/data" //Max Size 62
#define CONFIG_DIR_NAME "清理规则"

static int findPackageInProFile(char * package, char * config_file);

int main(int argc, char * argv[])
{
    if (getuid() != 0)
    {
        printf(L_NOT_USE_ROOT);
        return 1;
    }
    
    argc--;
    argv++;
    if (argc < 4)
    {
        printf(L_ARGS_FAILED);
        return 1;
    }
    
    char * work_dir = NULL;
    char * app_package = NULL;
    
    // Get work_dir & package
    while (argc > 1)
    {
        if (strcmp(argv[0], "-w") == 0)
        {
            if (access(argv[1], F_OK) != 0)
            {
                printf(L_CONFIG_PATH_NOTFIND);
                return 1;
            }
            if (strlen(argv[1]) > MAX_WORK_DIR_LEN)
            {
                printf(L_CONFIG_PATH_TOOLONG);
                return 1;
            }
            work_dir = argv[1];
            argc -= 2;
            argv += 2;
        }
        else if (strcmp(argv[0], "-p") == 0)
        {
            if (strlen(argv[1]) > MAX_PACKAGE - 2) // -2 for '\0'
            {
                printf(L_PACKAGE_TOOLONG, MAX_PACKAGE);
                return 1;
            }
            app_package = argv[1];
            argc -= 2;
            argv += 2;
        }
        else
        {
            printf(L_ARGS_FAILED_2);
            return 1;
        }
    }
    if (work_dir == NULL)
    {
        printf(L_ARG_CONFIGPATH_ERR);
        return 1;
    }
    if (app_package == NULL)
    {
        printf(L_ARG_PACKAGE_ERR);
        return 1;
    }
    
    // config dir
    char config_dir[strlen(work_dir) + strlen(CONFIG_DIR_NAME) + 2];
    snprintf(config_dir, sizeof(config_dir), "%s/%s", work_dir, CONFIG_DIR_NAME);
    if (access(config_dir, F_OK) != 0)
    {
        printf(L_AC_CONFIG_NOTFINF);
        return 1;
    }
    
    // 遍历配置目录
    int get_config = 0; // 已成功读取目标配置。需要声明在目录读取循环外部，不然后面读不到
    struct dirent * config_name;
    DIR * config_dir_fp = opendir(config_dir);
    if (config_dir_fp == NULL)
    {
        printf(L_OPEN_PATH_FAILED, config_dir);
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
        snprintf(config_file, sizeof(config_file), "%s/%s", config_dir, config_name -> d_name);
        
        // 这个函数目标是检查配置文件是否为目标配置文件
        if (findPackageInProFile(app_package, config_file) != 1)
        {
            continue;
        }
        
        FILE * config_fp = fopen(config_file, "r");
        if (config_fp == NULL)
        {
            closedir(config_dir_fp);
            continue;
            // 这里本来应直接退出，这是异常行为
            // 因为 findPackageInProFile 函数不应返回 1
        }
        
        // 遍历配置文件
        get_config = 0; // 配置读取标志清零，新配置新的开始
        int count = 0;
        char app_dir[64 + MAX_PACKAGE];
        char len_str[MAX_PATH] = "", app_name[MAX_APP_NAME] = "";
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
            if (get_config == 0)
            {
                char * len_str_p = len_str;
                while (isspace(* len_str_p)) len_str_p++;
                if (* len_str_p == '@')
                {
                    app_package_fp = strtok(len_str, "/");
                    app_name_fp = strtok(NULL, "/");
                    if (app_package_fp && app_name_fp)
                    {
                        snprintf(app_name, sizeof(app_name), "%s", app_name_fp);                  //软件名称
                        snprintf(app_dir, sizeof(app_dir), "%s/%s", DATA_DIR, app_package_fp + 1);   //软件目录
                        
                        if (access(app_dir, F_OK) == 0)
                        {
                            printf(L_AC_CLEAR, app_name);
                            get_config = 1; //注意这里设置了已读取配置信息标志
                            continue;
                        }
                        else
                        {
                            printf(L_AC_CONFIG_APP_NOTFIND, config_name -> d_name);
                            break;
                        }
                    }
                    else
                    {
                        if (app_package_fp == NULL)
                        {
                            printf(L_AC_CONFIG_PACKAGE_ERR, config_name -> d_name);
                        }
                        else if (app_name_fp == NULL)
                        {
                            printf(L_AC_CONFIG_APPNAME_ERR, config_name -> d_name);
                        }
                        break;
                    }
                }
                else
                {
                    printf(L_AC_CONFIG_ERR, config_name -> d_name);
                    break;
                }
            }
            else if (get_config == 1) // 1: 已读取文件声明信息
            {
                char app_cf_dir[strlen(app_dir) + strlen(len_str) + 2];
                snprintf(app_cf_dir, sizeof(app_cf_dir), "%s/%s", app_dir, len_str);
                
                char * len_str_ptr = len_str;
                while(isspace(* len_str_ptr)) len_str_ptr++;
                // 如果该行被注释则返回
                if (* len_str_ptr == '#')
                {
                    continue;
                }
                // 配置不应以／开头！
                if (* len_str_ptr == '/')
                {
                    printf(L_AC_CONFIG_ERR_1, config_name -> d_name, count);
                    continue;
                }
                // 防止路径逃逸
                if (strstr(len_str, "../"))
                {
                    printf(L_AC_CONFIG_ERR_2, config_name -> d_name, count);
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
                    printf(L_AC_CLEAR_PATH_ERR_1, len_str);
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
                        printf(L_AC_CLEAR_PATH_ERR_2, len_str);
                        continue;
                    }
                    if (WIFEXITED(end) && WEXITSTATUS(end) != 0)
                    {
                        printf(L_AC_CLEAR_PATH_ERR_3, len_str);
                        continue;
                    }
                }
            }
            else
            {
                printf(L_AC_CONFIG_ERR, config_name -> d_name);
                break;
            }
        }
        fclose(config_fp);
    }
    closedir(config_dir_fp);
    
    if (get_config == 1)
    {
        printf(L_AC_CLEAN_SUCCESSFUL);
    }
    else
    {
        printf(L_AC_CLEAN_FAILED);
    }
}

/*
检查配置是否包含传入包名（预期配置）
接收：
    char * package 包名
    char * config_file 配置文件（完整路径）
返回：
    int 异常返回-1，找到返回1，未找到返回0
*/
static int findPackageInProFile(char * package, char * config_file)
{
    int end = 0;
    char config_len[MAX_PACKAGE + 64] = "";
    if (access(config_file, R_OK) != 0)
    {
        return -1;
    }
    FILE * config_file_fp = fopen(config_file, "r");
    if (config_file_fp == NULL)
    {
        return -1;
    }
    while (fgets(config_len, sizeof(config_len), config_file_fp))
    {
        config_len[strcspn(config_len, "\n")] = 0;
        if (strstr(config_len, package))
        {
            char * config_len_p = config_len;
            while (isspace(* config_len_p)) config_len_p++;
            if (* config_len_p == '@')
            {
                end = 1;
            }
        }
    }
    fclose(config_file_fp);
    return end;
}
