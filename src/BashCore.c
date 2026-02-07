// 此Code来自ClearBox模块，用于根据传入参数调用清理脚本
#include "INCLUDE/BashCore.h"

#define PATH_ROM_FILE "/data/adb/wipe_cache/PATH"
#define SETTINGS_FILE_NAME "settings.prop" //Max Size 14
#define BASH_DIR "bin"
#define SERVER_NAME "BashCore"

// 这里每个声明均是一个子功能函数
static int running(char * args[]);
static int module_config(char * home_dir, char * mode, char * config_file);
static int app_cache_clean(char * home_dir, char * work_dir);
static int system_cache_clean(char * home_dir, char * work_dir);
static int cust_rule_clean(char * home_dir, char * work_dir);
static int storage_clean(char * home_dir, char * work_dir);
static int file_clean(char * home_dir, char * work_dir, char * str);
static int app_cust_rule_clean(char * home_dir, char * work_dir, char * str);
static int file_all(char * home_dir, char * work_dir);
static int file_all_auto(char * home_dir, char * work_dir);
static int set_install(char * home_dir, char * work_dir, char * bin_dir, char * str);
static int set_storage(char * home_dir, char * work_dir, char * bin_dir, char * str);
static int f2fs_gc(char * home_dir);
static int fast_gc(char * home_dir);
static int dexoat_system(char * home_dir);
static int dexoat_cust(char * home_dir, char * str);
static int freezer_open(char * home_dir);

int main(int argc, char * argv[])
{
    if (getuid() != 0)
    {
        fprintf(stderr, L_NOT_USE_ROOT);
        return 1;
    }
    if (argc < 2)
    {
        fprintf(stderr, L_ARGS_FAILED);
        return 1;
    }
    
    // 设置命名空间
    if (set_name_space() != 0)
    {
        return 1;
    }
    
    // Get PATH
    if (access(PATH_ROM_FILE, F_OK) != 0)
    {
        fprintf(stderr, " » Error：Read PATH\n");
        return 1;
    }
    
    char home_dir[128] = "",
         work_dir[128] = "",
         bin_dir[128] = "",
         path_file_line[256] = "";
    char * path_file_key = NULL;
    char * path_file_value = NULL;
    
    // 读取模块 PATH
    int path_file_err = 0;
    FILE * path_file_fp = fopen(PATH_ROM_FILE, "r");
    if (path_file_fp == NULL)
    {
        fprintf(stderr, " » Error：Read PATH\n");
        return 1;
    }
    while (fgets(path_file_line, sizeof(path_file_line), path_file_fp))
    {
        path_file_key = strtok(path_file_line, "=");
        path_file_value = strtok(NULL, "=");
        
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
        else if (strcmp(path_file_key, "bin_dir") == 0)
        {
            snprintf(bin_dir, sizeof(bin_dir), "%s", path_file_value);
            bin_dir[strcspn(bin_dir, "\n")] = 0;
        }
        else
        {
            path_file_err = 1; //如果有错误行则设置标识自动重写纠正
        }
    }
    fclose(path_file_fp);
    if (access(home_dir, F_OK) != 0)
    {
        fprintf(stderr, " » Error：HOME_PATH\n");
        return 1;
    }
    if (access(work_dir, F_OK) != 0)
    {
        fprintf(stderr, " » Error：WORK_PATH\n");
        return 1;
    }
    if (access(bin_dir, F_OK) != 0)
    {
        fprintf(stderr, " » Error：BIN_PATH\n");
        return 1;
    }
    
    /* 
    尝试重写纠正
    如果PATH内容有误可能会重写成错误的
    这里仅防止意外行，比如PATH=/xxx影响运行
    */
    if (path_file_err == 1)
    {
        fprintf(stderr, " » PATH file error, automatically retrying the rewrite... please try again.\n");
        FILE * fp = fopen(PATH_ROM_FILE, "w");
        if (fp)
        {
            fprintf(fp, "home_dir=%s\nwork_dir=%s\nbin_dir=%s", home_dir, work_dir, bin_dir);
            fclose(fp);
        }
        return 1;
    }
    
    // 根据输入参数执行对应操作
    if (strcasecmp(argv[1], "ClearAll") == 0)
    {
        pid_t pids[5];
        for (int i = 0; i < 5; i++)
        {
            pids[i] = fork();
            if (pids[i] == 0)
            {
                switch (i)
                {
                    case 0: 
                        fast_gc(home_dir);
                        break;
                    case 1: 
                        app_cache_clean(home_dir, work_dir);
                        break;
                    case 2: 
                        storage_clean(home_dir, work_dir);
                        break;
                    case 3: 
                        cust_rule_clean(home_dir, work_dir);
                        break;
                    case 4: 
                        freezer_open(home_dir);
                        break;
                }
                exit(0);
            }
        }
        for (int i = 0; i < 5; i++)
        {
            waitpid(pids[i], NULL, 0);
        }
        
        // 文件归类是高资源占用操作不并行
        file_all_auto(home_dir, work_dir);
        
        write_log(work_dir, SERVER_NAME, "优化清理");
    }
    else if (strcasecmp(argv[1], "ClearCache") == 0)
    {
        if (app_cache_clean(home_dir, work_dir) != 0)
        {
            write_log(work_dir, SERVER_NAME, "清理第三方软件缓存失败");
        }
    }
    else if (strcasecmp(argv[1], "Clear_SCache") == 0)
    {
        if (system_cache_clean(home_dir, work_dir) != 0)
        {
            write_log(work_dir, SERVER_NAME, "清理系统软件缓存失败");
        }
    }
    else if (strcasecmp(argv[1], "List_Dir") == 0)
    {
        if (cust_rule_clean(home_dir, work_dir) != 0)
        {
            write_log(work_dir, SERVER_NAME, "自定义规则清理失败");
        }
    }
    else if (strcasecmp(argv[1], "All_Dir") == 0)
    {
        if (storage_clean(home_dir, work_dir) != 0)
        {
            write_log(work_dir, SERVER_NAME, "清理储存目录失败");
        }
    }
    else if (strcasecmp(argv[1], "File_Clear") == 0)
    {
        if (argc < 2)
        {
            fprintf(stderr, L_ARGS_FAILED_2);
        }
        else if (file_clean(home_dir, work_dir, argv[2]) != 0)
        {
            write_log(work_dir, SERVER_NAME, "自定义文件清理失败");
        }
    }
    else if (strcasecmp(argv[1], "Clear_App") == 0)
    {
        if (argc < 2)
        {
            fprintf(stderr, L_ARGS_FAILED_2);
        }
        else if (app_cust_rule_clean(home_dir, work_dir, argv[2]) != 0)
        {
            write_log(work_dir, SERVER_NAME, "自定义软件清理失败");
        }
    }
    else if (strcasecmp(argv[1], "File_All") == 0)
    {
        if (file_all(home_dir, work_dir) != 0)
        {
            write_log(work_dir, SERVER_NAME, "自定义文件归类失败");
        }
    }
    else if (strcasecmp(argv[1], "StopInstall") == 0)
    {
        if (argc < 2)
        {
            fprintf(stderr, L_ARGS_FAILED_2);
        }
        else if (set_install(home_dir, work_dir, bin_dir, argv[2]) != 0)
        {
            write_log(work_dir, SERVER_NAME, "阻止安装失败");
        }
    }
    else if (strcasecmp(argv[1], "StopStorage") == 0)
    {
        if (argc < 2)
        {
            fprintf(stderr, L_ARGS_FAILED_2);
        }
        else if (set_storage(home_dir, work_dir, bin_dir, argv[2]) != 0)
        {
            write_log(work_dir, SERVER_NAME, "内部储存固定失败");
        }
    }
    else if (strcasecmp(argv[1], "F2fs_GC") == 0)
    {
        if (f2fs_gc(home_dir) != 0)
        {
            write_log(work_dir, SERVER_NAME, "F2FS GC 失败");
        }
    }
    else if (strcasecmp(argv[1], "Dexoat_1") == 0)
    {
        if (dexoat_system(home_dir) != 0)
        {
            write_log(work_dir, SERVER_NAME, "运行系统Dexoat失败");
        }
    }
    else if (strcasecmp(argv[1], "Dexoat_2") == 0)
    {
        if (argc < 2)
        {
            fprintf(stderr, L_ARGS_FAILED_2);
        }
        else if (dexoat_cust(home_dir, argv[2]) != 0)
        {
            write_log(work_dir, SERVER_NAME, "自定义模式Dexoat失败");
        }
    }
    else if (strcasecmp(argv[1], "Dexoat_3") == 0)
    {
        if (dexoat_cust(home_dir, "space") != 0)
        {
            write_log(work_dir, SERVER_NAME, "Dexoat RESET 失败");
        }
    }
    else if (strcasecmp(argv[1], "Freezer") == 0)
    {
        freezer_open(home_dir);
    }
    else if (strcasecmp(argv[1], "configManager") == 0)
    {
        if (argc < 3)
        {
            fprintf(stderr, L_ARGS_FAILED_2);
        }
        else
        {
            module_config(home_dir, argv[2], argv[3]);
        }
    }
    else
    {
        fprintf(stderr, L_ARGS_FAILED_2);
    }
    
    return 0;
}

// 一个通用函数，用途：执行二进制/脚本
static int running(char * args[])
{
    pid_t newPid = fork();
    if (newPid == -1)
    {
        return 1;
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
            return 1;
        }
        if (WIFEXITED(end) && WEXITSTATUS(end) != 0)
        {
            return 1;
        }
    }
    return 0;
}

/*
接下来每个函数都是一个子功能
传给 running 函数的相关参数由函数内部拼接
*/

// 配置备份 & 恢复
static int module_config(char * home_dir, char * mode, char * config_file)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/ConfigManager.sh", home_dir);
    char * args[] = {"sh", bash, mode, config_file, NULL};
    return running(args);
}

// 清理第三方软件缓存
static int app_cache_clean(char * home_dir, char * work_dir)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/CacheClean", home_dir, BASH_DIR);
    char * args[] = {bash, "-w", work_dir, "-m", "AppCache_3", NULL};
    return running(args);
}

// 清理系统软件缓存
static int system_cache_clean(char * home_dir, char * work_dir)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/CacheClean", home_dir, BASH_DIR);
    char * args[] = {bash, "-w", work_dir, "-m", "AppCache_S", NULL};
    return running(args);
}

// 运行处理自定义规则
static int cust_rule_clean(char * home_dir, char * work_dir)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/StorageRuleClean", home_dir, BASH_DIR);
    char * args[] = {bash, "-w", work_dir, NULL};
    return running(args);
}

// 清理储存目录
static int storage_clean(char * home_dir, char * work_dir)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/StorageClean", home_dir, BASH_DIR);
    char * args[] = {bash, "-w", work_dir, NULL};
    return running(args);
}

// 文件清理
static int file_clean(char * home_dir, char * work_dir, char * str)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/FileManager", home_dir, BASH_DIR);
    char * args[] = {bash, "-w", work_dir, "-m", "fileclean", "-n", str, NULL};
    return running(args);
}

// 自定义软件 规则清理
static int app_cust_rule_clean(char * home_dir, char * work_dir, char * str)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/AppClean", home_dir, BASH_DIR);
    char * args[] = {bash, "-p", str, "-w", work_dir, NULL};
    return running(args);
}

// 文件归类
static int file_all(char * home_dir, char * work_dir)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/FileManager", home_dir, BASH_DIR);
    char * args[] = {bash, "-w", work_dir, "-m", "fileall", "-n", "null", NULL};
    return running(args);
}

// 根据prop决定是否运行文件归类（仅用于一键/自动清理 该选项打开状态
static int file_all_auto(char * home_dir, char * work_dir)
{
    int fileall = 0;
    char * key = NULL;
    char * value = NULL;
    char line[256] = "", settingsFile[strlen(work_dir) + 16];
    snprintf(settingsFile, sizeof(settingsFile), "%s/%s", work_dir, SETTINGS_FILE_NAME);
    FILE * settingsFile_fp = fopen(settingsFile, "r");
    if (settingsFile_fp)
    {
        while (fgets(line, sizeof(line), settingsFile_fp))
        {
            key = strtok(line, "=");
            value = strtok(NULL, "=");
            
            if (strcmp(key, "fileall") == 0)
            {
                if (strcmp(value, "1") == 0)
                {
                    fileall = 1;
                }
            }
        }
        fclose(settingsFile_fp);
    }
    if (fileall == 1)
    {
        char bash[128] = "";
        snprintf(bash, sizeof(bash), "%s/%s/FileManager", home_dir, BASH_DIR);
        char * args[] = {bash, "-w", work_dir, "-m", "fileall", "-n", "null", NULL};
        running(args);
    }
    return 0;
}

// 阻止安装
static int set_install(char * home_dir, char * work_dir, char * bin_dir, char * str)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/SetInstall", home_dir, BASH_DIR);
    char * args[] = {bash, "-w", work_dir, "-b", bin_dir, "-s", str, NULL};
    return running(args);
}

// 内部储存固定
static int set_storage(char * home_dir, char * work_dir, char * bin_dir, char * str)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/SetStorage", home_dir, BASH_DIR);
    char * args[] = {bash, "-w", work_dir, "-b", bin_dir, "-s", str, NULL};
    return running(args);
}

// 磁盘GC
static int f2fs_gc(char * home_dir)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/F2fs_GC", home_dir, BASH_DIR);
    char * args[] = {bash, "F2FS_GC", NULL};
    return running(args);
}

// 快速GC
static int fast_gc(char * home_dir)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/F2fs_GC", home_dir, BASH_DIR);
    char * args[] = {bash, "FAST_GC", NULL};
    return running(args);
}

// Dexoat 模式1：触发系统Dexoat
static int dexoat_system(char * home_dir)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/Dexoat", home_dir, BASH_DIR);
    char * args[] = {bash, "SYSTEM_DEXOAT", NULL};
    return running(args);
}

// Dexoat 模式2：自定义模式Dexoat
static int dexoat_cust(char * home_dir, char * str)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/Dexoat", home_dir, BASH_DIR);
    char * args[] = {bash, "FAST_DEXOAT", str, NULL};
    return running(args);
}

// 其它优化，打开原生墓碑
static int freezer_open(char * home_dir)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/FreeZer", home_dir, BASH_DIR);
    char * args[] = {bash, NULL};
    return running(args);
}
