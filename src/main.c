/*
                    GNU GENERAL PUBLIC
                        Version 3

     此 Code 来自 ClearBox 模块，用于根据传入参数调用清理脚本
*/

#include "INCLUDE/main.h"

#define PATH_ROM_FILE "/data/adb/wipe_cache/PATH" // PATH 储存文件
#define SERVER_NAME "clearbox"                        // 进程名

static int running(char * args[]);
static int module_config(char * home_dir, char * mode, char * config_file);
static int file_all(char * work_dir, char * settings_file, int auto_);
static int fast_gc(char * argv[], char * settings_file, int auto_);

LangType current_lang;

int main(int argc, char * argv[])
{
    // 多语言
    char lang[PROP_VALUE_MAX] = "";
    FILE * local_lang_p = popen("settings get system system_locales", "r");
    if (local_lang_p)
    {
        fgets(lang, sizeof(lang), local_lang_p);
        lang[strcspn(lang, "\n")] = '\0';
        char * p = strchr(lang, ',');
        if (p)
        {
            * p = '\0';
        }
        pclose(local_lang_p);
    }
    if (strstr(lang, "zh-CN") ||
       strstr(lang, "Hans"))
    {
        current_lang = LANG_zh_CN;
    }
    else if (strstr(lang, "zh-TW") ||
            strstr(lang, "Hant"))
    {
        current_lang = LANG_zh_TW;
    }
    else
    {
        current_lang = LANG_en_US;
    }

    if (getuid() != 0)
    {
        fprintf(stderr, L_NOT_USE_ROOT, getuid());
        return -1;
    }
    if (argc < 2)
    {
        fprintf(stderr, L_ARGS_FAILED);
        return -1;
    }
    
    // 设置命名空间
    if (set_name_space() != 0)
    {
        return -1;
    }
    
    char home_dir[128] = "",        // 模块根目录
         work_dir[128] = "",        // 配置目录
         path_file_line[256] = "";
    
    char * path_file_key = NULL;
    char * path_file_value = NULL;
    int path_file_err = 0;
    
    // 读取 PATH
    FILE * path_file_fp = fopen(PATH_ROM_FILE, "r");
    if (path_file_fp == NULL)
    {
        fprintf(stderr, L_OPEN_FILE_FAILED, PATH_ROM_FILE, strerror(errno));
        return -1;
    }
    while (fgets(path_file_line, sizeof(path_file_line), path_file_fp))
    {
        char * path_file_line_p = NULL;
        path_file_key = strtok_r(path_file_line, "=", &path_file_line_p);
        path_file_value = strtok_r(NULL, "=", &path_file_line_p);
        
        if (strcmp(path_file_key, "home_dir") == 0)
        {
            snprintf(home_dir, sizeof(home_dir), "%s", path_file_value);
            home_dir[strcspn(home_dir, "\n")] = 0;
        }
        else if (strcmp(path_file_key, "work_dir") == 0)
        {
            snprintf(work_dir, sizeof(work_dir), "%s", path_file_value);
            work_dir[strcspn(work_dir, "\n")] = 0;
        }
        else
        {
            path_file_err = 1; // 如果有错误行则设置标识
        }
    }
    fclose(path_file_fp);
    
    if (access(home_dir, F_OK) != 0)
    {
        fprintf(stderr, " » Error：HOME_PATH\n");
        return -1;
    }
    if (access(work_dir, F_OK) != 0)
    {
        fprintf(stderr, " » Error：WORK_PATH\n");
        return -1;
    }
    
    /* 
    PATH 错误时尝试重写纠正
    如果原 PATH 内容有误可能会重写成错误的
    这里仅防止意外行，比如 PATH=/xxx 影响后续可能的其它脚本运行
    */
    if (path_file_err == 1)
    {
        fprintf(stderr, L_PATH_FILE_ERROR, PATH_ROM_FILE);
        FILE * fp = fopen(PATH_ROM_FILE, "w");
        if (fp)
        {
            fprintf(fp, "home_dir=%s\nwork_dir=%s", home_dir, work_dir);
            fclose(fp);
        }
        else
        {
            fprintf(stderr, L_OPEN_FILE_FAILED, PATH_ROM_FILE, strerror(errno));
        }
        return -1;
    }
    
    // 定义设置配置文件
    char settings_file[strlen(work_dir) + strlen(SETTINGS_FILE) + 2];
    snprintf(settings_file, sizeof(settings_file), "%s/%s", work_dir, SETTINGS_FILE);
    
    // 根据输入参数执行对应操作
    if (strcasecmp(argv[1], "ClearAll") == 0)
    {
        pid_t pids[6];
        for (int i = 0; i < 6; i++)
        {
            pids[i] = fork();
            if (pids[i] == 0)
            {
                switch (i)
                {
                    case 0: 
                        app_cust_rule_clean(work_dir, "null", 1);
                        break;
                    case 1: 
                        fast_gc(argv, settings_file, 1);
                        break;
                    case 2: 
                        app_cache_clean(work_dir, 0);
                        break;
                    case 3: 
                        storage_clean(work_dir);
                        break;
                    case 4: 
                        cust_rule_clean(work_dir);
                        break;
                    case 5:
                        freezer_open();
                        break;
                }
                exit(0);
            }
        }
        for (int i = 0; i < 6; i++)
        {
            waitpid(pids[i], NULL, 0);
        }
        
        // 文件归类是高资源占用操作不并行
        file_all(work_dir, settings_file, 1);
    }
    else if (strcasecmp(argv[1], "ClearCache") == 0)
    {
        app_cache_clean(work_dir, 0);
    }
    else if (strcasecmp(argv[1], "Clear_SCache") == 0)
    {
        app_cache_clean(work_dir, 1);
    }
    else if (strcasecmp(argv[1], "List_Dir") == 0)
    {
        cust_rule_clean(work_dir);
    }
    else if (strcasecmp(argv[1], "All_Dir") == 0)
    {
        storage_clean(work_dir);
    }
    else if (strcasecmp(argv[1], "File_Clear") == 0)
    {
        if (argc < 3)
        {
            fprintf(stderr, L_ARGS_FAILED);
        }
        
        file_manager(work_dir, 0, argv[2]);
    }
    else if (strcasecmp(argv[1], "Clear_App") == 0)
    {
        if (argc < 3)
        {
            fprintf(stderr, L_ARGS_FAILED);
        }
        
        app_cust_rule_clean(work_dir, argv[2], 0);
    }
    else if (strcasecmp(argv[1], "Clear_App_All") == 0)
    {
        app_cust_rule_clean(work_dir, "null", 1);
    }
    else if (strcasecmp(argv[1], "File_All") == 0)
    {
        file_all(work_dir, settings_file, 0);
    }
    else if (strcasecmp(argv[1], "StopInstall") == 0)
    {
        if (argc < 3)
        {
            fprintf(stderr, L_ARGS_FAILED);
        }
        
        set_install(work_dir, argv[2]);
    }
    else if (strcasecmp(argv[1], "StopStorage") == 0)
    {
        if (argc < 3)
        {
            fprintf(stderr, L_ARGS_FAILED);
        }
        
        set_storage(work_dir, argv[2]);
    }
    else if (strcasecmp(argv[1], "Fast_GC") == 0)
    {
        fast_gc(argv, settings_file, 0);
    }
    else if (strcasecmp(argv[1], "F2fs_GC") == 0)
    {
        disk_gc(argv, 0);
    }
    else if (strcasecmp(argv[1], "Dexoat_1") == 0)
    {
        dexoat(0, "null");
    }
    else if (strcasecmp(argv[1], "Dexoat_2") == 0)
    {
        if (argc < 3)
        {
            fprintf(stderr, L_ARGS_FAILED);
        }
        
        dexoat(1, argv[2]);
    }
    else if (strcasecmp(argv[1], "Dexoat_3") == 0)
    {
        dexoat(1, "space");
    }
    else if (strcasecmp(argv[1], "Freezer") == 0)
    {
        freezer_open();
    }
    else if (strcasecmp(argv[1], "StopCached") == 0)
    {
        stop_cache_daemon(argv, work_dir);
    }
    else if (strcasecmp(argv[1], "Timed") == 0)
    {
        time_daemon(argv, work_dir);
    }
    else if (strcasecmp(argv[1], "BindPath") == 0)
    {
        if (argc < 3)
        {
            fprintf(stderr, L_ARGS_FAILED);
        }
        
        bmount(work_dir, argv[2]);
    }
    else if (strcasecmp(argv[1], "configManager") == 0)
    {
        if (argc < 3)
        {
            fprintf(stderr, L_ARGS_FAILED);
        }
        else if (strcmp(argv[2], "backup") == 0)
        {
            module_config(home_dir, argv[2], "");
        }
        else
        {
            if (argc < 4)
            {
                fprintf(stderr, L_ARGS_FAILED);
            }
            module_config(home_dir, argv[2], argv[3]);
        }
    }
    else if (strcasecmp(argv[1], "help") == 0 ||
            strcasecmp(argv[1], "-help") == 0 ||
            strcasecmp(argv[1], "--help") == 0)
    {
        help(argv);
    }
    else
    {
        fprintf(stderr, L_MODE_ERR, argv[1]);
        return -1;
    }
    
    return 0;
}

// 一个通用函数，用途：执行二进制/脚本
static int running(char * args[])
{
    pid_t newPid = fork();
    if (newPid == -1)
    {
        return -1;
    }
    if (newPid == 0)
    {
        execvp(args[0], args);
        _exit(1);
    }
    else
    {
        int end = 0;
        if (waitpid(newPid, &end, 0) == -1)
        {
            return -1;
        }
        if (WIFEXITED(end) && WEXITSTATUS(end) != 0)
        {
            return -1;
        }
    }
    return 0;
}

// 配置备份 & 恢复
static int module_config(char * home_dir, char * mode, char * config_file)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/ConfigManager.sh", home_dir);
    char * args[] = {"sh", bash, mode, config_file, NULL};
    return running(args);
}

// 文件归类
static int file_all(char * work_dir, char * settings_file, int auto_)
{
    if (auto_ == 1) // 根据prop决定是否运行文件归类（仅用于一键/自动清理
    {
        int auto_file_all = get_settings_prop(settings_file, "clearbox_auto_file_all", NULL, 0);
        if (auto_file_all != 1)
        {
            return 0;
        }
    }
    return file_manager(work_dir, 1, "null");
}

// 快速 GC
static int fast_gc(char * argv[], char * settings_file, int auto_)
{
    if (auto_ == 1)
    {
        int auto_fast_gc = get_settings_prop(settings_file, "clearbox_auto_fast_gc", NULL, 0);
        if (auto_fast_gc != 1)
        {
            return 0;
        }
    }
    return disk_gc(argv, 1);
}
