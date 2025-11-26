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

static int Run(char * cmd, char * str, char * str2);

static int configFunction(char * home_dir, char * mode, char * config_file);
static int clearCache(char * home_dir);
static int clearSystemCache(char * home_dir);
static int listDir(char * home_dir);
static int allDir(char * home_dir);
static int dirFile(char * home_dir);
static int clearTar(char * home_dir, char * work_dir);
static int fileClear(char * home_dir, char * str);
static int clearApp(char * home_dir, char * str);
static int fileAll(char * home_dir);
static int fileAll2(char * home_dir, char * work_dir);
static int stopInstall(char * home_dir, char * str);
static int stopStorage(char * home_dir, char * str);
static int f2fsGC(char * home_dir);
static int fastGC(char * home_dir);
static int DexoatSYSTEM_DEXOAT(char * home_dir);
static int DexoatFAST_DEXOAT(char * home_dir, char * str);
static int DexoatRESET(char * home_dir);
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
    char log_file[strlen(work_dir) + 32];
    log_file[0] = '\0';
    snprintf(log_file, sizeof(log_file), "%s/运行日志.log", work_dir);
    FILE * log_file_fp = fopen(log_file, "a");
    if (log_file_fp == NULL)
    {
        log_file_fp = stderr;
        in_log_file = false;
    }
    
    // 根据输入参数执行对应操作
    if (strcasecmp(COM[1], "ClearAll") == 0)
    {
        clearCache(home_dir);
        dirFile(home_dir);
        clearTar(home_dir, work_dir);
        fileAll2(home_dir, work_dir);
        fastGC(home_dir);
        FreeZer(home_dir);
        fprintf(log_file_fp, "[%s] 一键优化清理\n", now_time);
    }
    else if (strcasecmp(COM[1], "ClearCache") == 0)
    {
        clearCache(home_dir);
        fprintf(log_file_fp, "[%s] 清理第三方软件缓存\n", now_time);
    }
    else if (strcasecmp(COM[1], "Clear_SCache") == 0)
    {
        clearSystemCache(home_dir);
        fprintf(log_file_fp, "[%s] 清理系统软件缓存\n", now_time);
    }
    else if (strcasecmp(COM[1], "List_Dir") == 0)
    {
        listDir(home_dir);
        fprintf(log_file_fp, "[%s] 自定义规则清理\n", now_time);
    }
    else if (strcasecmp(COM[1], "All_Dir") == 0)
    {
        allDir(home_dir);
        fprintf(log_file_fp, "[%s] 清理储存目录\n", now_time);
    }
    else if (strcasecmp(COM[1], "File_Clear") == 0)
    {
        fileClear(home_dir, COM[2]);
        fprintf(log_file_fp, "[%s] 自定义文件清理\n", now_time);
    }
    else if (strcasecmp(COM[1], "Clear_App") == 0)
    {
        clearApp(home_dir, COM[2]);
        fprintf(log_file_fp, "[%s] 自定义软件清理\n", now_time);
    }
    else if (strcasecmp(COM[1], "File_All") == 0)
    {
        fileAll(home_dir);
        fprintf(log_file_fp, "[%s] 自定义文件归类\n", now_time);
    }
    else if (strcasecmp(COM[1], "StopInstall") == 0)
    {
        stopInstall(home_dir, COM[2]);
        fprintf(log_file_fp, "[%s] %s阻止安装\n", now_time, COM[2]);
    }
    else if (strcasecmp(COM[1], "StopStorage") == 0)
    {
        stopStorage(home_dir, COM[2]);
        fprintf(log_file_fp, "[%s] %s内部储存固定\n", now_time, COM[2]);
    }
    else if (strcasecmp(COM[1], "F2fs_GC") == 0)
    {
        f2fsGC(home_dir);
        fprintf(log_file_fp, "[%s] F2FS GC\n", now_time);
    }
    else if (strcasecmp(COM[1], "Dexoat_1") == 0)
    {
        DexoatSYSTEM_DEXOAT(home_dir);
        fprintf(log_file_fp, "[%s] 触发系统Dexoat\n", now_time);
    }
    else if (strcasecmp(COM[1], "Dexoat_2") == 0)
    {
        DexoatFAST_DEXOAT(home_dir, COM[2]);
        fprintf(log_file_fp, "[%s] 自定义模式Dexoat\n", now_time);
    }
    else if (strcasecmp(COM[1], "Dexoat_3") == 0)
    {
        DexoatRESET(home_dir);
        fprintf(log_file_fp, "[%s] Dexoat RESET\n", now_time);
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
static int Run(char * cmd, char * str, char * str2)
{
    if (access(cmd, F_OK))
    {
        return 1;
    }
    
    char * command_args[5] = {NULL};
    if (strstr(cmd, ".sh"))
    {
        command_args[0] = "bash";
        command_args[1] = cmd;
        command_args[2] = str;
        command_args[3] = str2;
        command_args[4] = NULL;
    }
    else
    {
        command_args[0] = cmd;
        command_args[1] = str;
        command_args[2] = str2;
        command_args[3] = NULL;
    }
    
    pid_t newPid = fork();
    if (newPid == -1)
    {
        return 1;
    }
    if (newPid == 0)
    {
        execvp(command_args[0], command_args);
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
    //定义命令 & Log文件
    char bash[strlen(home_dir) + 32];
    bash[0] = '\0';
    snprintf(bash, sizeof(bash), "%s/ConfigManager.sh", home_dir);
    return Run(bash, mode, config_file);
}

// 清理第三方软件缓存
static int clearCache(char * home_dir)
{
    //定义命令 & Log文件
    char bash[strlen(home_dir) + 32];
    bash[0] = '\0';
    snprintf(bash, sizeof(bash), "%s/wipe_cache/data_cache", home_dir);
    return Run(bash, "", "");
}

// 清理系统软件缓存
static int clearSystemCache(char * home_dir)
{
    char bash[strlen(home_dir) + 32];
    bash[0] = '\0';
    snprintf(bash, sizeof(bash), "%s/wipe_cache/system_cache", home_dir);
    return Run(bash, "", "");
}

// 运行处理自定义规则
static int listDir(char * home_dir)
{
    char bash[strlen(home_dir) + 32];
    bash[0] = '\0';
    snprintf(bash, sizeof(bash), "%s/wipe_cache/wipe_list_dir", home_dir);
    return Run(bash, "", "");
}

// 清理储存目录
static int allDir(char * home_dir)
{
    char bash[strlen(home_dir) + 32];
    bash[0] = '\0';
    snprintf(bash, sizeof(bash), "%s/wipe_cache/wipe_all_dir.sh", home_dir);
    return Run(bash, "", "");
}

// 运行规则清理、清理储存目录
static int dirFile(char * home_dir)
{
    listDir(home_dir);
    allDir(home_dir);
    return 0;
}

// 根据prop配置，清理全部文件（仅用于一键/自动清理 该选项打开状态
static int clearTar(char * home_dir, char * work_dir)
{
    int clearall = 0;
    char * line_key = NULL;
    char settings_file[strlen(work_dir) + 32], settings_file_line[64] = "";
    settings_file[0] = '\0';
    snprintf(settings_file, sizeof(settings_file), "%s/settings.prop", work_dir);
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
        bash_1[0] = '\0';
        bash_2[0] = '\0';
        snprintf(bash_1, sizeof(bash_1), "%s/wipe_cache/ClearService1.sh", home_dir);
        snprintf(bash_2, sizeof(bash_2), "%s/wipe_cache/ClearService2.sh", home_dir);
        Run(bash_1, "", "");
        Run(bash_2, "", "");
    }
    
    return 0;
}

// 自定义格式文件清理
static int fileClear(char * home_dir, char * str)
{
    char bash_1[strlen(home_dir) + 64], bash_2[strlen(home_dir) + 64];
    bash_1[0] = '\0';
    bash_2[0] = '\0';
    snprintf(bash_1, sizeof(bash_1), "%s/wipe_cache/ClearService1.sh", home_dir);
    snprintf(bash_2, sizeof(bash_2), "%s/wipe_cache/ClearService2.sh", home_dir);
    Run(bash_1, str, "");
    Run(bash_2, str, "");
    return 0;
}

// 自定义软件 规则清理
static int clearApp(char * home_dir, char * str)
{
    char bash[strlen(home_dir) + 64];
    bash[0] = '\0';
    snprintf(bash, sizeof(bash), "%s/wipe_cache/AppClean", home_dir);
    return Run(bash, str, "");
}

// 自定义格式文件归类
static int fileAll(char * home_dir)
{
    char bash_1[strlen(home_dir) + 32], bash_2[strlen(home_dir) + 32];
    bash_1[0] = '\0';
    bash_2[0] = '\0';
    snprintf(bash_1, sizeof(bash_1), "%s/wipe_cache/file_1.sh", home_dir);
    snprintf(bash_2, sizeof(bash_2), "%s/wipe_cache/file_2.sh", home_dir);
    Run(bash_1, "", "");
    Run(bash_2, "", "");
    return 0;
}

// 根据prop决定是否运行文件归类（仅用于一键/自动清理 该选项打开状态
static int fileAll2(char * home_dir, char * work_dir)
{
    int fileall = 0;
    char * key = NULL;
    char temp[64] = "", settingsFile[strlen(work_dir) + 32];
    snprintf(settingsFile, sizeof(settingsFile), "%s/settings.prop", work_dir);
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
        bash_1[0] = '\0';
        bash_2[0] = '\0';
        snprintf(bash_1, sizeof(bash_1), "%s/wipe_cache/file_1.sh", home_dir);
        snprintf(bash_2, sizeof(bash_2), "%s/wipe_cache/file_2.sh", home_dir);
        Run(bash_1, "", "");
        Run(bash_2, "", "");
    }
    
    return 0;
}

// 阻止安装
static int stopInstall(char * home_dir, char * str)
{
    char bash[strlen(home_dir) + 64];
    bash[0] = '\0';
    snprintf(bash, sizeof(bash), "%s/wipe_cache/StopInstall.sh", home_dir);
    return Run(bash, str, "");
}

// 内部储存固定
static int stopStorage(char * home_dir, char * str)
{
    char bash[strlen(home_dir) + 64];
    bash[0] = '\0';
    snprintf(bash, sizeof(bash), "%s/wipe_cache/StopStorage.sh", home_dir);
    return Run(bash, str, "");
}

// 磁盘GC
static int f2fsGC(char * home_dir)
{
    char bash[strlen(home_dir) + 32];
    bash[0] = '\0';
    snprintf(bash, sizeof(bash), "%s/wipe_cache/f2fs_GC.sh", home_dir);
    return Run(bash, "F2FS_GC", "");
}

// 快速GC
static int fastGC(char * home_dir)
{
    char bash[strlen(home_dir) + 32];
    bash[0] = '\0';
    snprintf(bash, sizeof(bash), "%s/wipe_cache/f2fs_gc.sh", home_dir);
    return Run(bash, "FAST_GC", "");
}

// Dexoat 模式1：触发系统Dexoat
static int DexoatSYSTEM_DEXOAT(char * home_dir)
{
    char bash[strlen(home_dir) + 32];
    bash[0] = '\0';
    snprintf(bash, sizeof(bash), "%s/wipe_cache/Dexoat.sh", home_dir);
    return Run(bash, "SYSTEM_DEXOAT", "");
}

// Dexoat 模式2：自定义模式Dexoat
static int DexoatFAST_DEXOAT(char * home_dir, char * str)
{
    char bash[strlen(home_dir) + 64];
    bash[0] = '\0';
    snprintf(bash, sizeof(bash), "%s/wipe_cache/Dexoat.sh", home_dir);
    return Run(bash, "FAST_DEXOAT", str);
}

// Dexoat 模式3：Dexoat还原
static int DexoatRESET(char * home_dir)
{
    char bash[strlen(home_dir) + 64];
    bash[0] = '\0';
    snprintf(bash, sizeof(bash), "%s/wipe_cache/Dexoat.sh", home_dir);
    return Run(bash, "RESET", "");
}

// 其它优化，打开原生墓碑
static int FreeZer(char * home_dir)
{
    char bash[strlen(home_dir) + 32];
    bash[0] = '\0';
    snprintf(bash, sizeof(bash), "%s/wipe_cache/FreeZer.sh", home_dir);
    return Run(bash, "", "");
}