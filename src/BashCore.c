// 此Code来自ClearBox模块，用于根据传入参数调用清理脚本
#include "INCLUDE/BashCore.h"

#define PATH_ROM_FILE "/data/adb/wipe_cache/PATH"
#define SETTINGS_FILE_NAME "settings.prop" //Max Size 14
#define LOG_FILE_NAME "运行日志.log" //Max Size 30
#define BASH_DIR "bin"

// 这里每个声明均是一个子功能函数
static int Run(char * args[]);
static int configFunction(char * home_dir, char * mode, char * config_file);
static int clearCache(char * home_dir, char * work_dir);
static int clearSystemCache(char * home_dir, char * work_dir);
static int listDir(char * home_dir, char * work_dir);
static int allDir(char * home_dir, char * work_dir);
static int fileClear(char * home_dir, char * work_dir, char * str);
static int clearApp(char * home_dir, char * work_dir, char * str);
static int fileAll(char * home_dir, char * work_dir);
static int fileAll2(char * home_dir, char * work_dir);
static int stopInstall(char * home_dir, char * work_dir, char * bin_dir, char * str);
static int stopStorage(char * home_dir, char * work_dir, char * bin_dir, char * str);
static int f2fsGC(char * home_dir);
static int fastGC(char * home_dir);
static int DexoatSYSTEM_DEXOAT(char * home_dir);
static int DexoatCUST_DEXOAT(char * home_dir, char * str);
static int FreeZer(char * home_dir);

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
            fprintf(fp, "home_dir=%s\nwork_dir=%s\nbin_dir=%s", 
                        home_dir, work_dir, bin_dir
                  );
            fclose(fp);
        }
        return 1;
    }
    
    /* 临时关闭一下Selinux
    属实是没办法，后期可能写一下规则
    否则测试莫名其妙的Selinux拦截还是太多
    */
    int off_selinux = 0; // 状态记录
    char check_selinux[16] = "";
    FILE * check_selinux_fp = popen("getenforce", "r");
    if (check_selinux_fp)
    {
        fgets(check_selinux, sizeof(check_selinux), check_selinux_fp);
        check_selinux[strcspn(check_selinux, "\n")] = 0;
        
        if (strcasecmp(check_selinux, "Enforcing") == 0)
        {
            if (system("setenforce 0") == 0)
            {
                off_selinux = 1;
            }
        }
        pclose(check_selinux_fp);
    }
    
    //获取当前时间（用于log）
    char now_time[64] = "";
    time_t now_time_tmp = time(NULL);
    struct tm * t_tmp = localtime(&now_time_tmp);
    strftime(now_time, sizeof(now_time), " %m-%d %H:%M:%S ", t_tmp);
    
    //定义及打开Log文件
    bool in_log_file = true;
    char log_file[strlen(work_dir) + 32];
    snprintf(log_file, sizeof(log_file), "%s/%s", work_dir, LOG_FILE_NAME);
    FILE * log_file_fp = fopen(log_file, "a");
    if (log_file_fp == NULL)
    {
        log_file_fp = stderr;
        in_log_file = false;
    }
    
    // 根据输入参数执行对应操作
    if (strcasecmp(argv[1], "ClearAll") == 0)
    {
        clearCache(home_dir, work_dir);
        listDir(home_dir, work_dir);
        allDir(home_dir, work_dir);
        fileAll2(home_dir, work_dir);
        fastGC(home_dir);
        FreeZer(home_dir);
        fprintf(log_file_fp, "[%s] I 一键优化清理\n", now_time);
    }
    else if (strcasecmp(argv[1], "ClearCache") == 0)
    {
        if (clearCache(home_dir, work_dir) == 0)
        {
            fprintf(log_file_fp, "[%s] I 清理第三方软件缓存\n", now_time);
        }
        else
        {
            fprintf(log_file_fp, "[%s] E 清理第三方软件缓存失败\n", now_time);
        }
    }
    else if (strcasecmp(argv[1], "Clear_SCache") == 0)
    {
        if (clearSystemCache(home_dir, work_dir) == 0)
        {
            fprintf(log_file_fp, "[%s] I 清理系统软件缓存\n", now_time);
        }
        else
        {
            fprintf(log_file_fp, "[%s] E 清理系统软件缓存失败\n", now_time);
        }
    }
    else if (strcasecmp(argv[1], "List_Dir") == 0)
    {
        if (listDir(home_dir, work_dir) == 0)
        {
            fprintf(log_file_fp, "[%s] I 自定义规则清理\n", now_time);
        }
        else
        {
            fprintf(log_file_fp, "[%s] E 自定义规则清理失败\n", now_time);
        }
    }
    else if (strcasecmp(argv[1], "All_Dir") == 0)
    {
        if (allDir(home_dir, work_dir) == 0)
        {
            fprintf(log_file_fp, "[%s] I 清理储存目录\n", now_time);
        }
        else
        {
            fprintf(log_file_fp, "[%s] E 清理储存目录失败\n", now_time);
        }
    }
    else if (strcasecmp(argv[1], "File_Clear") == 0)
    {
        if (argc < 2)
        {
            fprintf(stderr, L_ARGS_FAILED_2);
        }
        else if (fileClear(home_dir, work_dir, argv[2]) == 0)
        {
            fprintf(log_file_fp, "[%s] I 自定义文件清理\n", now_time);
        }
        else
        {
            fprintf(log_file_fp, "[%s] E 自定义文件清理失败\n", now_time);
        }
    }
    else if (strcasecmp(argv[1], "Clear_App") == 0)
    {
        if (argc < 2)
        {
            fprintf(stderr, L_ARGS_FAILED_2);
        }
        else if (clearApp(home_dir, work_dir, argv[2]) == 0)
        {
            fprintf(log_file_fp, "[%s] I 自定义软件清理\n", now_time);
        }
        else
        {
            fprintf(log_file_fp, "[%s] E 自定义软件清理失败\n", now_time);
        }
    }
    else if (strcasecmp(argv[1], "File_All") == 0)
    {
        if (fileAll(home_dir, work_dir) == 0)
        {
            fprintf(log_file_fp, "[%s] I 自定义文件归类\n", now_time);
        }
        else
        {
            fprintf(log_file_fp, "[%s] E 自定义文件归类失败\n", now_time);
        }
    }
    else if (strcasecmp(argv[1], "StopInstall") == 0)
    {
        if (argc < 2)
        {
            fprintf(stderr, L_ARGS_FAILED_2);
        }
        else if (stopInstall(home_dir, work_dir, bin_dir, argv[2]) == 0)
        {
            fprintf(log_file_fp, "[%s] I %s 阻止安装\n", now_time, argv[2]);
        }
        else
        {
            fprintf(log_file_fp, "[%s] E %s 阻止安装失败\n", now_time, argv[2]);
        }
    }
    else if (strcasecmp(argv[1], "StopStorage") == 0)
    {
        if (argc < 2)
        {
            fprintf(stderr, L_ARGS_FAILED_2);
        }
        else if (stopStorage(home_dir, work_dir, bin_dir, argv[2]) == 0)
        {
            fprintf(log_file_fp, "[%s] I %s 内部储存固定\n", now_time, argv[2]);
        }
        else
        {
            fprintf(log_file_fp, "[%s] E %s 内部储存固定失败\n", now_time, argv[2]);
        }
    }
    else if (strcasecmp(argv[1], "F2fs_GC") == 0)
    {
        if (f2fsGC(home_dir) == 0)
        {
            fprintf(log_file_fp, "[%s] I F2FS GC\n", now_time);
        }
        else
        {
            fprintf(log_file_fp, "[%s] E F2FS GC 失败\n", now_time);
        }
    }
    else if (strcasecmp(argv[1], "Dexoat_1") == 0)
    {
        if (DexoatSYSTEM_DEXOAT(home_dir) == 0)
        {
            fprintf(log_file_fp, "[%s] I 触发系统Dexoat\n", now_time);
        }
        else
        {
            fprintf(log_file_fp, "[%s] E 触发系统Dexoat失败\n", now_time);
        }
    }
    else if (strcasecmp(argv[1], "Dexoat_2") == 0)
    {
        if (argc < 2)
        {
            fprintf(stderr, L_ARGS_FAILED_2);
        }
        else if (DexoatCUST_DEXOAT(home_dir, argv[2]) == 0)
        {
            fprintf(log_file_fp, "[%s] I 自定义模式Dexoat\n", now_time);
        }
        else
        {
            fprintf(log_file_fp, "[%s] E 自定义模式Dexoat失败\n", now_time);
        }
    }
    else if (strcasecmp(argv[1], "Dexoat_3") == 0)
    {
        if (DexoatCUST_DEXOAT(home_dir, "space") == 0)
        {
            fprintf(log_file_fp, "[%s] I Dexoat RESET\n", now_time);
        }
        else
        {
            fprintf(log_file_fp, "[%s] E Dexoat RESET 失败\n", now_time);
        }
    }
    else if (strcasecmp(argv[1], "Freezer") == 0)
    {
        FreeZer(home_dir);
    }
    else if (strcasecmp(argv[1], "configManager") == 0)
    {
        if (argc < 3)
        {
            fprintf(stderr, L_ARGS_FAILED_2);
        }
        else
        {
            configFunction(home_dir, argv[2], argv[3]);
        }
    }
    else
    {
        fprintf(stderr, L_ARGS_FAILED_2);
    }
    
    //ON the Selinux
    if (off_selinux == 1) system("setenforce 1");
    if (in_log_file) fclose(log_file_fp);
    return 0;
}

// 一个通用函数，用途：执行二进制/脚本
static int Run(char * args[])
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
传给 Run 函数的相关参数由函数内部拼接
*/

// 配置备份 & 恢复
static int configFunction(char * home_dir, char * mode, char * config_file)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/ConfigManager.sh", home_dir);
    char * args[] = {"bash", bash, mode, config_file, NULL};
    return Run(args);
}

// 清理第三方软件缓存
static int clearCache(char * home_dir, char * work_dir)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/CacheClean", home_dir, BASH_DIR);
    char * args[] = {bash, "-w", work_dir, "-m", "AppCache_3", NULL};
    return Run(args);
}

// 清理系统软件缓存
static int clearSystemCache(char * home_dir, char * work_dir)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/CacheClean", home_dir, BASH_DIR);
    char * args[] = {bash, "-w", work_dir, "-m", "AppCache_S", NULL};
    return Run(args);
}

// 运行处理自定义规则
static int listDir(char * home_dir, char * work_dir)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/StorageRuleClean", home_dir, BASH_DIR);
    char * args[] = {bash, "-w", work_dir, NULL};
    return Run(args);
}

// 清理储存目录
static int allDir(char * home_dir, char * work_dir)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/StorageClean", home_dir, BASH_DIR);
    char * args[] = {bash, "-w", work_dir, NULL};
    return Run(args);
}

// 文件清理
static int fileClear(char * home_dir, char * work_dir, char * str)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/FileManager", home_dir, BASH_DIR);
    char * args[] = {bash, "-w", work_dir, "-m", "fileclean", "-n", str, NULL};
    return Run(args);
}

// 自定义软件 规则清理
static int clearApp(char * home_dir, char * work_dir, char * str)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/AppClean", home_dir, BASH_DIR);
    char * args[] = {bash, "-p", str, "-w", work_dir, NULL};
    return Run(args);
}

// 文件归类
static int fileAll(char * home_dir, char * work_dir)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/FileManager", home_dir, BASH_DIR);
    char * args[] = {bash, "-w", work_dir, "-m", "fileall", "-n", "null", NULL};
    return Run(args);
}

// 根据prop决定是否运行文件归类（仅用于一键/自动清理 该选项打开状态
static int fileAll2(char * home_dir, char * work_dir)
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
        Run(args);
    }
    return 0;
}

// 阻止安装
static int stopInstall(char * home_dir, char * work_dir, char * bin_dir, char * str)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/StopInstall", home_dir, BASH_DIR);
    char * args[] = {bash, "-w", work_dir, "-b", bin_dir, "-s", str, NULL};
    return Run(args);
}

// 内部储存固定
static int stopStorage(char * home_dir, char * work_dir, char * bin_dir, char * str)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/StopStorage", home_dir, BASH_DIR);
    char * args[] = {bash, "-w", work_dir, "-b", bin_dir, "-s", str, NULL};
    return Run(args);
}

// 磁盘GC
static int f2fsGC(char * home_dir)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/F2fs_GC", home_dir, BASH_DIR);
    char * args[] = {bash, "F2FS_GC", NULL};
    return Run(args);
}

// 快速GC
static int fastGC(char * home_dir)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/F2fs_GC", home_dir, BASH_DIR);
    char * args[] = {bash, "FAST_GC", NULL};
    return Run(args);
}

// Dexoat 模式1：触发系统Dexoat
static int DexoatSYSTEM_DEXOAT(char * home_dir)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/Dexoat", home_dir, BASH_DIR);
    char * args[] = {bash, "SYSTEM_DEXOAT", NULL};
    return Run(args);
}

// Dexoat 模式2：自定义模式Dexoat
static int DexoatCUST_DEXOAT(char * home_dir, char * str)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/Dexoat", home_dir, BASH_DIR);
    char * args[] = {bash, "FAST_DEXOAT", str, NULL};
    return Run(args);
}

// 其它优化，打开原生墓碑
static int FreeZer(char * home_dir)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/FreeZer", home_dir, BASH_DIR);
    char * args[] = {bash, NULL};
    return Run(args);
}
