//By ClearBox
// 此Core来自ClearBox模块，用于根据传入参数调用清理脚本
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>

#define HOME_DIR "/data/adb/modules/wipe_cache"
#define WORK_DIR "/data/adb/wipe_cache"
#define BIN_DIR_1 "/data/adb/magisk"
#define BIN_DIR_2 "/data/adb/ap/bin"
#define BIN_DIR_3 "/data/adb/ksu/bin"
#define SETTINGS_FILE_NAME "settings.prop"
#define LOG_FILE_NAME "运行日志.log"
#define BASH_DIR "wipe_cache"

static int Run(char * args[]);
static int configFunction(char * bin_dir, char * mode, char * config_file);
static int clearCache();
static int clearSystemCache();
static int listDir();
static int allDir(char * bin_dir);
static int fileClear(char * str);
static int clearApp(char * str);
static int fileAll();
static int fileAll2();
static int stopInstall(char * bin_dir, char * str);
static int stopStorage(char * bin_dir, char * str);
static int f2fsGC();
static int fastGC();
static int DexoatSYSTEM_DEXOAT();
static int DexoatCUST_DEXOAT(char * str);
static int FreeZer();

int main(int COMI, char * COM[])
{
    if (getuid() != 0)
    {
        printf(" » Please use root privileges!\n");
        return 1;
    }
    if (COMI < 2)
    {
        printf(" » ERROR：需要一个参数\n");
        return 1;
    }
    
    //bin_dir定义
    char bin_dir[128] = "";
    if (access(BIN_DIR_1, F_OK) == 0)
    {
        snprintf(bin_dir, sizeof(bin_dir), "%s", BIN_DIR_1);
    }
    else if (access(BIN_DIR_2, F_OK) == 0)
    {
        snprintf(bin_dir, sizeof(bin_dir), "%s", BIN_DIR_2);
    }
    else if (access(BIN_DIR_3, F_OK) == 0)
    {
        snprintf(bin_dir, sizeof(bin_dir), "%s", BIN_DIR_3);
    }
    else
    {
        return 1;
    }
    
    //Check and Off the Selinux
    int off_selinux = 0;
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
    strftime(now_time, sizeof(now_time), "%m-%d %H:%M:%S", t_tmp);
    
    //定义并预打开Log文件
    bool in_log_file = true;
    char log_file[128] = "";
    log_file[0] = '\0';
    snprintf(log_file, sizeof(log_file), "%s/%s", WORK_DIR, LOG_FILE_NAME);
    FILE * log_file_fp = fopen(log_file, "a");
    if (log_file_fp == NULL)
    {
        log_file_fp = stderr;
        in_log_file = false;
    }
    
    // 根据输入参数执行对应操作
    if (strcasecmp(COM[1], "ClearAll") == 0)
    {
        clearCache();
        listDir();
        allDir(bin_dir);
        fileAll2();
        fastGC();
        FreeZer();
        fprintf(log_file_fp, "I [%s] 一键优化清理\n", now_time);
    }
    else if (strcasecmp(COM[1], "ClearCache") == 0)
    {
        if (clearCache() == 0)
        {
            fprintf(log_file_fp, "I [%s] 清理第三方软件缓存\n", now_time);
        }
        else
        {
            fprintf(log_file_fp, "E [%s] 清理第三方软件缓存失败\n", now_time);
        }
    }
    else if (strcasecmp(COM[1], "Clear_SCache") == 0)
    {
        if (clearSystemCache() == 0)
        {
            fprintf(log_file_fp, "I [%s] 清理系统软件缓存\n", now_time);
        }
        else
        {
            fprintf(log_file_fp, "E [%s] 清理系统软件缓存失败\n", now_time);
        }
    }
    else if (strcasecmp(COM[1], "List_Dir") == 0)
    {
        if (listDir() == 0)
        {
            fprintf(log_file_fp, "I [%s] 自定义规则清理\n", now_time);
        }
        else
        {
            fprintf(log_file_fp, "E [%s] 自定义规则清理失败\n", now_time);
        }
    }
    else if (strcasecmp(COM[1], "All_Dir") == 0)
    {
        if (allDir(bin_dir) == 0)
        {
            fprintf(log_file_fp, "I [%s] 清理储存目录\n", now_time);
        }
        else
        {
            fprintf(log_file_fp, "E [%s] 清理储存目录失败\n", now_time);
        }
    }
    else if (strcasecmp(COM[1], "File_Clear") == 0)
    {
        if (fileClear(COM[2]) == 0)
        {
            fprintf(log_file_fp, "I [%s] 自定义文件清理\n", now_time);
        }
        else
        {
            fprintf(log_file_fp, "E [%s] 自定义文件清理失败\n", now_time);
        }
    }
    else if (strcasecmp(COM[1], "Clear_App") == 0)
    {
        if (clearApp(COM[2]) == 0)
        {
            fprintf(log_file_fp, "I [%s] 自定义软件清理\n", now_time);
        }
        else
        {
            fprintf(log_file_fp, "E [%s] 自定义软件清理失败\n", now_time);
        }
    }
    else if (strcasecmp(COM[1], "File_All") == 0)
    {
        if (fileAll() == 0)
        {
            fprintf(log_file_fp, "I [%s] 自定义文件归类\n", now_time);
        }
        else
        {
            fprintf(log_file_fp, "E [%s] 自定义文件归类失败\n", now_time);
        }
    }
    else if (strcasecmp(COM[1], "StopInstall") == 0)
    {
        if (stopInstall(bin_dir, COM[2]) == 0)
        {
            fprintf(log_file_fp, "I [%s] %s 阻止安装\n", now_time, COM[2]);
        }
        else
        {
            fprintf(log_file_fp, "E [%s] %s 阻止安装失败\n", now_time, COM[2]);
        }
    }
    else if (strcasecmp(COM[1], "StopStorage") == 0)
    {
        if (stopStorage(bin_dir, COM[2]) == 0)
        {
            fprintf(log_file_fp, "I [%s] %s 内部储存固定\n", now_time, COM[2]);
        }
        else
        {
            fprintf(log_file_fp, "E [%s] %s 内部储存固定失败\n", now_time, COM[2]);
        }
    }
    else if (strcasecmp(COM[1], "F2fs_GC") == 0)
    {
        if (f2fsGC() == 0)
        {
            fprintf(log_file_fp, "I [%s] F2FS GC\n", now_time);
        }
        else
        {
            fprintf(log_file_fp, "E [%s] F2FS GC 失败\n", now_time);
        }
    }
    else if (strcasecmp(COM[1], "Dexoat_1") == 0)
    {
        if (DexoatSYSTEM_DEXOAT() == 0)
        {
            fprintf(log_file_fp, "I [%s] 触发系统Dexoat\n", now_time);
        }
        else
        {
            fprintf(log_file_fp, "E [%s] 触发系统Dexoat失败\n", now_time);
        }
    }
    else if (strcasecmp(COM[1], "Dexoat_2") == 0)
    {
        if (DexoatCUST_DEXOAT(COM[2]) == 0)
        {
            fprintf(log_file_fp, "I [%s] 自定义模式Dexoat\n", now_time);
        }
        else
        {
            fprintf(log_file_fp, "E [%s] 自定义模式Dexoat失败\n", now_time);
        }
    }
    else if (strcasecmp(COM[1], "Dexoat_3") == 0)
    {
        if (DexoatCUST_DEXOAT("space") == 0)
        {
            fprintf(log_file_fp, "I [%s] Dexoat RESET\n", now_time);
        }
        else
        {
            fprintf(log_file_fp, "E [%s] Dexoat RESET 失败\n", now_time);
        }
    }
    else if (strcasecmp(COM[1], "Freezer") == 0)
    {
        FreeZer();
    }
    else if (strcasecmp(COM[1], "configManager") == 0)
    {
        configFunction(bin_dir, COM[2], COM[3]);
    }
    else
    {
        printf(" » 传入参数错误！\n");
    }
    
    //ON the Selinux
    if (off_selinux == 1) system("setenforce 1");
    if (in_log_file) fclose(log_file_fp);
    
    return 0;
}

// 一个通用函数，用途：执行脚本
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

// 配置备份 & 恢复
static int configFunction(char * bin_dir, char * mode, char * config_file)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/ConfigManager.sh", HOME_DIR);
    char * args[] = {"bash", bash, WORK_DIR, bin_dir, mode, config_file, NULL};
    return Run(args);
}

// 清理第三方软件缓存
static int clearCache()
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/data_cache", HOME_DIR, BASH_DIR);
    char * args[] = {bash, "-w", WORK_DIR, NULL};
    return Run(args);
}

// 清理系统软件缓存
static int clearSystemCache()
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/system_cache", HOME_DIR, BASH_DIR);
    char * args[] = {bash, NULL};
    return Run(args);
}

// 运行处理自定义规则
static int listDir()
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/wipe_list_dir", HOME_DIR, BASH_DIR);
    char * args[] = {bash, "-w", WORK_DIR, NULL};
    return Run(args);
}

// 清理储存目录
static int allDir(char * bin_dir)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/wipe_all_dir", HOME_DIR, BASH_DIR);
    char * args[] = {bash, "-b", bin_dir, "-w", WORK_DIR, NULL};
    return Run(args);
}

// 文件清理
static int fileClear(char * str)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/ClearService", HOME_DIR, BASH_DIR);
    char * args[] = {bash, "-w", WORK_DIR, "-m", str, NULL};
    return Run(args);
}

// 自定义软件 规则清理
static int clearApp(char * str)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/AppClean", HOME_DIR, BASH_DIR);
    char * args[] = {bash, "-p", str, "-w", WORK_DIR, NULL};
    return Run(args);
}

// 文件归类
static int fileAll()
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/FileAll", HOME_DIR, BASH_DIR);
    char * args[] = {bash, "-w", WORK_DIR, NULL};
    return Run(args);
}

// 根据prop决定是否运行文件归类（仅用于一键/自动清理 该选项打开状态
static int fileAll2()
{
    int fileall = 0;
    char * key = NULL;
    char temp[64] = "", settingsFile[128] = "";
    snprintf(settingsFile, sizeof(settingsFile), "%s/%s", WORK_DIR, SETTINGS_FILE_NAME);
    FILE * settingsFile_fp = fopen(settingsFile, "r");
    if (settingsFile_fp)
    {
        while (fgets(temp, sizeof(temp), settingsFile_fp))
        {
            temp[strcspn(temp, "\n")] = 0;
            
            if (strstr(temp, "fileall="))
            {
                key = strtok(temp, "=");
                key = strtok(NULL, "=");
                if (strcmp(key, "1") == 0)
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
        snprintf(bash, sizeof(bash), "%s/%s/FileAll", HOME_DIR, BASH_DIR);
        char * args[] = {bash, "-w", WORK_DIR, NULL};
        Run(args);
    }
    return 0;
}

// 阻止安装
static int stopInstall(char * bin_dir, char * str)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/StopInstall", HOME_DIR, BASH_DIR);
    char * args[] = {bash, "-w", WORK_DIR, "-b", bin_dir, "-s", str, NULL};
    return Run(args);
}

// 内部储存固定
static int stopStorage(char * bin_dir, char * str)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/StopStorage", HOME_DIR, BASH_DIR);
    char * args[] = {bash, "-w", WORK_DIR, "-b", bin_dir, "-s", str, NULL};
    return Run(args);
}

// 磁盘GC
static int f2fsGC()
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/f2fs_GC", HOME_DIR, BASH_DIR);
    char * args[] = {bash, "F2FS_GC", NULL};
    return Run(args);
}

// 快速GC
static int fastGC()
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/f2fs_GC", HOME_DIR, BASH_DIR);
    char * args[] = {bash, "FAST_GC", NULL};
    return Run(args);
}

// Dexoat 模式1：触发系统Dexoat
static int DexoatSYSTEM_DEXOAT()
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/Dexoat", HOME_DIR, BASH_DIR);
    char * args[] = {bash, "SYSTEM_DEXOAT", NULL};
    return Run(args);
}

// Dexoat 模式2：自定义模式Dexoat
static int DexoatCUST_DEXOAT(char * str)
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/Dexoat", HOME_DIR, BASH_DIR);
    char * args[] = {bash, "FAST_DEXOAT", str, NULL};
    return Run(args);
}

// 其它优化，打开原生墓碑
static int FreeZer()
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/%s/FreeZer", HOME_DIR, BASH_DIR);
    char * args[] = {bash, NULL};
    return Run(args);
}
