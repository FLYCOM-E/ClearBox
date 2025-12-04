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

#define SETTINGS_FILE_NAME "settings.prop"
#define LOG_FILE_NAME "运行日志.log"
#define BASH_DIR "wipe_cache"

static int Run(char * args[]);
static int configFunction(char * home_dir, char * mode, char * config_file);
static int clearCache(char * home_dir, char * work_dir);
static int clearSystemCache(char * home_dir);
static int listDir(char * home_dir, char * work_dir);
static int allDir(char * home_dir);
static int clearTar(char * home_dir, char * work_dir);
static int fileClear(char * home_dir, char * str);
static int clearApp(char * home_dir, char * work_dir, char * str);
static int fileAll(char * home_dir);
static int fileAll2(char * home_dir, char * work_dir);
static int stopInstall(char * home_dir, char * work_dir, char * bin_dir, char * str);
static int stopStorage(char * home_dir, char * work_dir, char * bin_dir, char * str);
static int f2fsGC(char * home_dir);
static int fastGC(char * home_dir);
static int DexoatSYSTEM_DEXOAT(char * home_dir);
static int DexoatCUST_DEXOAT(char * home_dir, char * str);
static int FreeZer(char * home_dir);

int main(int COMI, char * COM[])
{
    if (getuid() != 0)
    {
        printf(" » Please use root privileges!\n");
        return 1;
    }
    if (system("ClearBox -v >>/dev/null 2>&1") != 0)
    {
        printf(" » 模块加载异常，请排查反馈！\n");
        return 1;
    }
    if (COMI < 2)
    {
        printf(" » ERROR：需要一个参数\n");
        return 1;
    }
    
    //home_dir定义
    char home_dir[64] = "";
    FILE * home_dir_fp = popen("ClearBox -h", "r");
    if (home_dir_fp)
    {
        fgets(home_dir, sizeof(home_dir), home_dir_fp);
        home_dir[strcspn(home_dir, "\n")] = 0;
        pclose(home_dir_fp);
    }
    
    //work_dir定义
    char work_dir[64] = "";
    FILE * work_dir_fp = popen("ClearBox -w", "r");
    if (work_dir_fp)
    {
        fgets(work_dir, sizeof(work_dir), work_dir_fp);
        work_dir[strcspn(work_dir, "\n")] = 0;
        pclose(work_dir_fp);
    }
    
    //bin_dir定义
    char bin_dir[64] = "";
    FILE * bin_dir_fp = popen("ClearBox -b", "r");
    if (bin_dir_fp)
    {
        fgets(bin_dir, sizeof(bin_dir), bin_dir_fp);
        bin_dir[strcspn(bin_dir, "\n")] = 0;
        pclose(bin_dir_fp);
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
    char log_file[strlen(work_dir) + 64];
    log_file[0] = '\0';
    snprintf(log_file, sizeof(log_file), "%s/%s", work_dir, LOG_FILE_NAME);
    FILE * log_file_fp = fopen(log_file, "a");
    if (log_file_fp == NULL)
    {
        log_file_fp = stderr;
        in_log_file = false;
    }
    
    // 根据输入参数执行对应操作
    if (strcasecmp(COM[1], "ClearAll") == 0)
    {
        clearCache(home_dir, work_dir);
        listDir(home_dir, work_dir);
        allDir(home_dir);
        clearTar(home_dir, work_dir);
        fileAll2(home_dir, work_dir);
        fastGC(home_dir);
        FreeZer(home_dir);
        fprintf(log_file_fp, "I [%s] 一键优化清理\n", now_time);
    }
    else if (strcasecmp(COM[1], "ClearCache") == 0)
    {
        if (clearCache(home_dir, work_dir) == 0)
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
        if (clearSystemCache(home_dir) == 0)
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
        if (listDir(home_dir, work_dir) == 0)
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
        if (allDir(home_dir) == 0)
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
        if (fileClear(home_dir, COM[2]) == 0)
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
        if (clearApp(home_dir, work_dir, COM[2]) == 0)
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
        if (fileAll(home_dir) == 0)
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
        if (stopInstall(home_dir, work_dir, bin_dir, COM[2]) == 0)
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
        if (stopStorage(home_dir, work_dir, bin_dir, COM[2]) == 0)
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
        if (f2fsGC(home_dir) == 0)
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
        if (DexoatSYSTEM_DEXOAT(home_dir) == 0)
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
        if (DexoatCUST_DEXOAT(home_dir, COM[2]) == 0)
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
        if (DexoatCUST_DEXOAT(home_dir, "space") == 0)
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
        FreeZer(home_dir);
    }
    else if (strcasecmp(COM[1], "configManager") == 0)
    {
        configFunction(home_dir, COM[2], COM[3]);
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
static int configFunction(char * home_dir, char * mode, char * config_file)
{
    char bash[strlen(home_dir) + 32];
    snprintf(bash, sizeof(bash), "%s/ConfigManager.sh", home_dir);
    char * args[] = {"bash", bash, mode, config_file, NULL};
    return Run(args);
}

// 清理第三方软件缓存
static int clearCache(char * home_dir, char * work_dir)
{
    char bash[strlen(home_dir) + 32];
    snprintf(bash, sizeof(bash), "%s/%s/data_cache", home_dir, BASH_DIR);
    char * args[] = {bash, "-w", work_dir, NULL};
    return Run(args);
}

// 清理系统软件缓存
static int clearSystemCache(char * home_dir)
{
    char bash[strlen(home_dir) + 32];
    snprintf(bash, sizeof(bash), "%s/%s/system_cache", home_dir, BASH_DIR);
    char * args[] = {bash, NULL};
    return Run(args);
}

// 运行处理自定义规则
static int listDir(char * home_dir, char * work_dir)
{
    char bash[strlen(home_dir) + 32];
    snprintf(bash, sizeof(bash), "%s/%s/wipe_list_dir", home_dir, BASH_DIR);
    char * args[] = {bash, "-w", work_dir, NULL};
    return Run(args);
}

// 清理储存目录
static int allDir(char * home_dir)
{
    char bash[strlen(home_dir) + 32];
    snprintf(bash, sizeof(bash), "%s/%s/wipe_all_dir.sh", home_dir, BASH_DIR);
    char * args[] = {"bash", bash, NULL};
    return Run(args);
}

// 根据prop配置，清理全部文件（仅用于一键/自动清理 该选项打开状态
static int clearTar(char * home_dir, char * work_dir)
{
    int clearall = 0;
    char * line_key = NULL;
    char settings_file[strlen(work_dir) + 32], settings_file_line[64] = "";
    snprintf(settings_file, sizeof(settings_file), "%s/%s", work_dir, SETTINGS_FILE_NAME);
    FILE * settings_file_fp = fopen(settings_file, "r");
    if (settings_file_fp)
    {
        while (fgets(settings_file_line, sizeof(settings_file_line), settings_file_fp))
        {
            settings_file_line[strcspn(settings_file_line, "\n")] = 0;
            if (strstr(settings_file_line, "clearall="))
            {
                line_key = strtok(settings_file_line, "=");
                line_key = strtok(NULL, "=");
                if (strcmp(line_key, "1") == 0)
                {
                    clearall = 1;
                }
            }
        }
        fclose(settings_file_fp);
    }
    
    if (clearall == 1)
    {
        char bash_1[strlen(home_dir) + 32], bash_2[strlen(home_dir) + 32];
        snprintf(bash_1, sizeof(bash_1), "%s/%s/ClearService1.sh", home_dir, BASH_DIR);
        snprintf(bash_2, sizeof(bash_2), "%s/%s/ClearService2.sh", home_dir, BASH_DIR);
        char * args_1[] = {"bash", bash_1, NULL};
        char * args_2[] = {"bash", bash_2, NULL};
        Run(args_1);
        Run(args_2);
    }
    return 0;
}

// 自定义格式文件清理
static int fileClear(char * home_dir, char * str)
{
    char bash_1[strlen(home_dir) + 64], bash_2[strlen(home_dir) + 64];
    snprintf(bash_1, sizeof(bash_1), "%s/%s/ClearService1.sh", home_dir, BASH_DIR);
    snprintf(bash_2, sizeof(bash_2), "%s/%s/ClearService2.sh", home_dir, BASH_DIR);
    char * args_1[] = {"bash", bash_1, str, NULL};
    char * args_2[] = {"bash", bash_2, str, NULL};
    Run(args_1);
    Run(args_2);
    return 0;
}

// 自定义软件 规则清理
static int clearApp(char * home_dir, char * work_dir, char * str)
{
    char bash[strlen(home_dir) + 64];
    snprintf(bash, sizeof(bash), "%s/%s/AppClean", home_dir, BASH_DIR);
    char * args[] = {bash, "-p", str, "-w", work_dir, NULL};
    return Run(args);
}

// 自定义格式文件归类
static int fileAll(char * home_dir)
{
    char bash_1[strlen(home_dir) + 32], bash_2[strlen(home_dir) + 32];
    snprintf(bash_1, sizeof(bash_1), "%s/%s/file_1.sh", home_dir, BASH_DIR);
    snprintf(bash_2, sizeof(bash_2), "%s/%s/file_2.sh", home_dir, BASH_DIR);
    char * args_1[] = {"bash", bash_1, NULL};
    char * args_2[] = {"bash", bash_2, NULL};
    Run(args_1);
    Run(args_2);
    return 0;
}

// 根据prop决定是否运行文件归类（仅用于一键/自动清理 该选项打开状态
static int fileAll2(char * home_dir, char * work_dir)
{
    int fileall = 0;
    char * key = NULL;
    char temp[64] = "", settingsFile[strlen(work_dir) + 32];
    snprintf(settingsFile, sizeof(settingsFile), "%s/%s", work_dir, SETTINGS_FILE_NAME);
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
        char bash_1[strlen(home_dir) + 32], bash_2[strlen(home_dir) + 32];
        snprintf(bash_1, sizeof(bash_1), "%s/%s/file_1.sh", home_dir, BASH_DIR);
        snprintf(bash_2, sizeof(bash_2), "%s/%s/file_2.sh", home_dir, BASH_DIR);
        char * args_1[] = {"bash", bash_1, NULL};
        char * args_2[] = {"bash", bash_2, NULL};
        Run(args_1);
        Run(args_2);
    }
    return 0;
}

// 阻止安装
static int stopInstall(char * home_dir, char * work_dir, char * bin_dir, char * str)
{
    char bash[strlen(home_dir) + 64];
    snprintf(bash, sizeof(bash), "%s/%s/StopInstall", home_dir, BASH_DIR);
    char * args[] = {bash, "-w", work_dir, "-b", bin_dir, "-s", str, NULL};
    return Run(args);
}

// 内部储存固定
static int stopStorage(char * home_dir, char * work_dir, char * bin_dir, char * str)
{
    char bash[strlen(home_dir) + 64];
    snprintf(bash, sizeof(bash), "%s/%s/StopStorage", home_dir, BASH_DIR);
    char * args[] = {bash, "-w", work_dir, "-b", bin_dir, "-s", str, NULL};
    return Run(args);
}

// 磁盘GC
static int f2fsGC(char * home_dir)
{
    char bash[strlen(home_dir) + 32];
    snprintf(bash, sizeof(bash), "%s/%s/f2fs_GC", home_dir, BASH_DIR);
    char * args[] = {bash, "F2FS_GC", NULL};
    return Run(args);
}

// 快速GC
static int fastGC(char * home_dir)
{
    char bash[strlen(home_dir) + 32];
    snprintf(bash, sizeof(bash), "%s/%s/f2fs_GC", home_dir, BASH_DIR);
    char * args[] = {bash, "FAST_GC", NULL};
    return Run(args);
}

// Dexoat 模式1：触发系统Dexoat
static int DexoatSYSTEM_DEXOAT(char * home_dir)
{
    char bash[strlen(home_dir) + 32];
    snprintf(bash, sizeof(bash), "%s/%s/Dexoat", home_dir, BASH_DIR);
    char * args[] = {bash, "SYSTEM_DEXOAT", NULL};
    return Run(args);
}

// Dexoat 模式2：自定义模式Dexoat
static int DexoatCUST_DEXOAT(char * home_dir, char * str)
{
    char bash[strlen(home_dir) + 64];
    snprintf(bash, sizeof(bash), "%s/%s/Dexoat", home_dir, BASH_DIR);
    char * args[] = {bash, "FAST_DEXOAT", str, NULL};
    return Run(args);
}

// 其它优化，打开原生墓碑
static int FreeZer(char * home_dir)
{
    char bash[strlen(home_dir) + 32];
    snprintf(bash, sizeof(bash), "%s/%s/FreeZer", home_dir, BASH_DIR);
    char * args[] = {bash, NULL};
    return Run(args);
}
