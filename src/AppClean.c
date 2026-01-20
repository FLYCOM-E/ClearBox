// 此Code来自ClearBox模块，用于自定义规则软件内部清理
#include "INCLUDE/BashCore.h"

#define MAX_APP_NAME 128
#define DATA_DIR "/data/data" //Max Size 62
#define CONFIG_DIR_NAME "清理规则"

static int findPackageInProFile(char * package, char * config_file);

int main(int argc, char * argv[])
{
    if (getuid() != 0)
    {
        fprintf(stderr, L_NOT_USE_ROOT);
        return 1;
    }
    
    argc--;
    argv++;
    if (argc < 4)
    {
        fprintf(stderr, L_ARGS_FAILED);
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
                fprintf(stderr, L_CONFIG_PATH_NOTFOUND);
                return 1;
            }
            if (strlen(argv[1]) > MAX_WORK_DIR_LEN)
            {
                fprintf(stderr, L_CONFIG_PATH_TOOLONG);
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
                fprintf(stderr, L_PACKAGE_TOOLONG, MAX_PACKAGE);
                return 1;
            }
            app_package = argv[1];
            argc -= 2;
            argv += 2;
        }
        else
        {
            fprintf(stderr, L_ARGS_FAILED_2);
            return 1;
        }
    }
    if (work_dir == NULL)
    {
        fprintf(stderr, L_ARG_CONFIGPATH_ERR);
        return 1;
    }
    if (app_package == NULL)
    {
        fprintf(stderr, L_ARG_PACKAGE_ERR);
        return 1;
    }
    
    // 拼接工作目录
    char config_dir[strlen(work_dir) + strlen(CONFIG_DIR_NAME) + 2];
    snprintf(config_dir, sizeof(config_dir), "%s/%s", work_dir, CONFIG_DIR_NAME);
    if (access(config_dir, F_OK) != 0)
    {
        fprintf(stderr, L_AC_CONFIG_NOTFOUND);
        return 1;
    }
    
    // 遍历配置目录
    int get_config = 0; // 已成功读取目标配置。需要声明在目录读取循环外部，不然后面读不到
    struct dirent * config_name;
    DIR * config_dir_fp = opendir(config_dir);
    if (config_dir_fp == NULL)
    {
        fprintf(stderr, L_OPEN_PATH_FAILED, config_dir);
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
        get_config = 0; // 清空配置读取标志
        int count = 0;
        char app_dir[64 + MAX_PACKAGE];
        char line[MAX_PATH] = "", app_name[MAX_APP_NAME] = "";
        char * app_package_fp = NULL, * app_name_fp = NULL;
        
        while (fgets(line, sizeof(line), config_fp))
        {
            line[strcspn(line, "\n")] = 0;
            count++;  //记录行数
            
            char * line_ptr = line;
            while (isspace(* line_ptr)) line_ptr++;
            
            // 如果该行被注释/为空则返回
            if (* line_ptr == '#' || 
                strlen(line_ptr) < 1)
            {
                continue;
            }
            
            /*
            配置文件第一行以＠开头
            格式：
            @<软件包名>/<软件名称>
            */
            if (get_config == 0) // = 0 意味着当前配置未读取到初始行
            {
                if (* line_ptr == '@')
                {
                    app_package_fp = strtok(line, "/");
                    app_name_fp = strtok(NULL, "/");
                    if (app_package_fp && app_name_fp) // 如果软件名称 & 软件包名为空或者填写有误则跳过
                    {
                        snprintf(app_name, sizeof(app_name), "%s", app_name_fp);                  //软件名称
                        snprintf(app_dir, sizeof(app_dir), "%s/%s", DATA_DIR, app_package_fp + 1);   //软件目录
                        
                        if (access(app_dir, F_OK) == 0)
                        {
                            printf(L_AC_CLEAR, app_name);
                            get_config = 1; //设置已读取配置信息标志
                            fflush(stdout);
                            continue;
                        }
                        else
                        {
                            fprintf(stderr, L_AC_CONFIG_APP_NOTFOUND, config_name -> d_name);
                            break;
                        }
                    }
                    else
                    {
                        if (app_package_fp == NULL)
                        {
                            fprintf(stderr, L_AC_CONFIG_PACKAGE_ERR, config_name -> d_name);
                        }
                        else if (app_name_fp == NULL)
                        {
                            fprintf(stderr, L_AC_CONFIG_APPNAME_ERR, config_name -> d_name);
                        }
                        break;
                    }
                }
                else
                {
                    fprintf(stderr, L_AC_CONFIG_ERR, config_name -> d_name);
                    break;
                }
            }
            else if (get_config == 1) // 1: 已读取文件声明信息，这里如果仍然不匹配则会报错跳过，保证配置第一行正确
            {
                char app_cf_dir[strlen(app_dir) + strlen(line) + 2];
                snprintf(app_cf_dir, sizeof(app_cf_dir), "%s/%s", app_dir, line);
                
                // 不允许绝对路径
                if (* line_ptr == '/')
                {
                    fprintf(stderr, L_AC_CONFIG_ERR_1, config_name -> d_name, count);
                    continue;
                }
                // 防止路径逃逸
                if (strstr(line, "../"))
                {
                    fprintf(stderr, L_AC_CONFIG_ERR_2, config_name -> d_name, count);
                    continue;
                }
                // 这可以避免很多报错
                if (access(app_cf_dir, F_OK) != 0)
                {
                    continue;
                }
                
                long clear_size = s_remove(app_cf_dir, 0);
                if (clear_size == -1)
                {
                    fprintf(stderr, L_AC_CLEAR_PATH_ERR, line);
                }
                else
                {
                    fprintf(stderr, L_AC_CLEAR_PATH_SUCCESS, line, (clear_size / 1024 / 1024));
                }
            }
            else
            {
                fprintf(stderr, L_AC_CONFIG_ERR, config_name -> d_name);
                break;
            }
            fflush(stdout);
        }
        fclose(config_fp);
    }
    closedir(config_dir_fp);
    
    // 多个匹配配置有一个错误都会失败
    if (get_config == 1)
    {
        printf(L_AC_CLEAN_SUCCESSFUL);
    }
    else
    {
        fprintf(stderr, L_AC_CLEAN_FAILED);
        return -1;
    }
    
    return 0;
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
