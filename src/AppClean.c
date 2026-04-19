// 此Code来自ClearBox模块，用于自定义规则软件内部清理
#include "INCLUDE/BashCore.h"

#define MAX_APP_NAME 128 // 软件名称最大长度
#define DATA_DIR "/data/data" //Max Size 62
#define CONFIG_DIR_NAME "清理规则" // 配置目录名称

static int read_clear(char * config_file, int * success_config, int * failed_config, int * total_clear_size);

int app_cust_rule_clean(char * work_dir, char * app_package, int mode)
{
    // 拼接工作目录
    char config_dir[strlen(work_dir) + strlen(CONFIG_DIR_NAME) + 2];
    snprintf(config_dir, sizeof(config_dir), "%s/%s", work_dir, CONFIG_DIR_NAME);
    if (access(config_dir, F_OK) != 0)
    {
        fprintf(stderr, L_AC_CONFIG_NOTFOUND);
        return 1;
    }
    
    // 遍历配置目录
    int success_config = 0,
        failed_config = 0,
        total_clear_size = 0; // 总清理大小，如模式为全部清理则为所有配置累计清理和（单位 M）
    
    if (mode == 1)
    {
        struct dirent * config_name;
        DIR * config_dir_fp = opendir(config_dir);
        if (config_dir_fp == NULL)
        {
            fprintf(stderr, L_OPEN_PATH_FAILED, config_dir, strerror(errno));
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
            
            // Cleaning
            read_clear(config_file, &success_config, &failed_config, &total_clear_size);
        }
        closedir(config_dir_fp);
    }
    else
    {
        //config_file: 配置文件
        char config_file[strlen(config_dir) + strlen(app_package) + 10];
        snprintf(config_file, sizeof(config_file), "%s/%s.conf", config_dir, app_package);
        if (access(config_file, F_OK) == 0)
        {
            read_clear(config_file, &success_config, &failed_config, &total_clear_size);
        }
    }
    
    fflush(stdout); // 强制刷新一次，避免输出杂乱
    if (success_config != 0)
    {
        fprintf(stderr, L_AC_CLEAN_SUCCESSFUL, total_clear_size);
    }
    else
    {
        fprintf(stderr, L_AC_CLEAN_FAILED, failed_config);
        return -1;
    }
    
    return 0;
}

/*
配置读取/清理执行函数
接收：
    char * config_file 配置文件（完整路径）
    int * success_config 成功处理配置数量
    int * failed_config 处理失败配置数量
    int * total_clear_size 总清理大小（MB）
返回：
    0 成功，1 失败
*/
static int read_clear(char * config_file, int * success_config, int * failed_config, int * total_clear_size)
{
    FILE * config_fp = fopen(config_file, "r");
    if (config_fp == NULL)
    {
        fprintf(stderr, L_OPEN_FILE_FAILED, config_file, strerror(errno));
        return 1;
    }
    
    // 遍历配置文件
    int count = 0, get_config = 0;
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
                char * line_p = NULL;
                app_package_fp = strtok_r(line, "/", &line_p);
                app_name_fp = strtok_r(NULL, "/", &line_p);
                
                if (app_package_fp && app_name_fp) // 如果软件名称 & 软件包名为空或者填写有误则跳过
                {
                    snprintf(app_name, sizeof(app_name), "%s", app_name_fp);                  //软件名称
                    snprintf(app_dir, sizeof(app_dir), "%s/%s", DATA_DIR, app_package_fp + 1);   //软件目录
                    
                    if (access(app_dir, F_OK) == 0)
                    {
                        printf(L_AC_CLEAR, app_name);
                        get_config++;
                        (* success_config)++;
                        fflush(stdout);
                        continue;
                    }
                    else
                    {
                        fprintf(stderr, L_AC_CONFIG_APP_NOTFOUND, config_file);
                        (* failed_config)++;
                        break;
                    }
                }
                else
                {
                    if (app_package_fp == NULL)
                    {
                        fprintf(stderr, L_AC_CONFIG_PACKAGE_ERR, config_file);
                    }
                    else if (app_name_fp == NULL)
                    {
                        fprintf(stderr, L_AC_CONFIG_APPNAME_ERR, config_file);
                    }
                    (* failed_config)++;
                    break;
                }
            }
            else
            {
                fprintf(stderr, L_AC_CONFIG_ERR, config_file);
                break;
            }
        }
        
        char app_cf_dir[strlen(app_dir) + strlen(line) + 2];
        snprintf(app_cf_dir, sizeof(app_cf_dir), "%s/%s", app_dir, line);
        
        // 不允许绝对路径
        if (* line_ptr == '/')
        {
            fprintf(stderr, L_AC_CONFIG_ERR_1, config_file, count);
            continue;
        }
        // 防止路径逃逸
        if (strstr(line, "/../"))
        {
            fprintf(stderr, L_AC_CONFIG_ERR_2, config_file, count);
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
            printf(L_AC_CLEAR_PATH_SUCCESS, line, (clear_size / 1024 / 1024));
            (* total_clear_size) += (clear_size / 1024 / 1024);
        }
        fflush(stdout);
    }
    fclose(config_fp);
    
    return get_config ? 0 : 1;
}
