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

static int clear_cache(char * home_dir);
static int clear_scache(char * home_dir);
static int list_dir(char * home_dir);
static int all_dir(char * home_dir);
static int dir_file(char * home_dir);
static int clear_tar(char * home_dir, char * work_dir);
static int FileClear(char * home_dir, char * str);
static int ClearApp(char * home_dir, char * str);
static int file_all(char * home_dir);
static int file_all2(char * home_dir, char * work_dir);
static int STOPInstall(char * home_dir, char * str);
static int STOPStorage(char * home_dir, char * str);
static int f2fs_GC(char * home_dir);
static int fast_GC(char * home_dir);
static int Dexoat_SYSTEM_DEXOAT(char * home_dir);
static int Dexoat_FAST_DEXOAT(char * home_dir, char * str);
static int freezer(char * home_dir);

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
    int OffSelinux = 0;
    char checkSelinux[16] = "";
    FILE * checkSelinux_fp = popen("getenforce", "r");
    if (checkSelinux_fp)
    {
        fgets(checkSelinux, sizeof(checkSelinux), checkSelinux_fp);
        checkSelinux[strcspn(checkSelinux, "\n")] = 0;
        
        if (strcmp(checkSelinux, "Enforcing") == 0)
        {
            if (system("setenforce 0") == 0)
            {
                OffSelinux = 1;
            }
        }
        pclose(checkSelinux_fp);
    }
    
    //获取当前时间（用于log）
    char nowTime[64] = "";
    time_t nowTimeC = time(NULL);
    struct tm * t = localtime(&nowTimeC);
    strftime(nowTime, sizeof(nowTime), "%m-%d %H:%M:%S", t);
    
    //定义并预打开Log文件
    bool logFile_i = true;
    char logFile[strlen(work_dir) + 32];
    logFile[0] = '\0';
    snprintf(logFile, sizeof(logFile), "%s/运行日志.log", work_dir);
    FILE * logFile_fp = fopen(logFile, "a");
    if (logFile_fp == NULL)
    {
        logFile_fp = stderr;
        logFile_i = false;
    }
    
    // 根据输入参数执行对应操作
    if (strcasecmp(COM[1], "ClearAll") == 0)
    {
        clear_cache(home_dir);
        dir_file(home_dir);
        clear_tar(home_dir, work_dir);
        file_all2(home_dir, work_dir);
        fast_GC(home_dir);
        freezer(home_dir);
        fprintf(logFile_fp, "[%s] 一键优化清理\n", nowTime);
    }
    else if (strcasecmp(COM[1], "ClearCache") == 0)
    {
        clear_cache(home_dir);
        fprintf(logFile_fp, "[%s] 清理第三方软件缓存\n", nowTime);
    }
    else if (strcasecmp(COM[1], "Clear_SCache") == 0)
    {
        clear_scache(home_dir);
        fprintf(logFile_fp, "[%s] 清理系统软件缓存\n", nowTime);
    }
    else if (strcasecmp(COM[1], "List_Dir") == 0)
    {
        list_dir(home_dir);
        fprintf(logFile_fp, "[%s] 自定义规则清理\n", nowTime);
    }
    else if (strcasecmp(COM[1], "All_Dir") == 0)
    {
        all_dir(home_dir);
        fprintf(logFile_fp, "[%s] 清理储存目录\n", nowTime);
    }
    else if (strcasecmp(COM[1], "File_Clear") == 0)
    {
        FileClear(home_dir, COM[2]);
        fprintf(logFile_fp, "[%s] 自定义文件清理\n", nowTime);
    }
    else if (strcasecmp(COM[1], "Clear_App") == 0)
    {
        ClearApp(home_dir, COM[2]);
        fprintf(logFile_fp, "[%s] 自定义软件清理\n", nowTime);
    }
    else if (strcasecmp(COM[1], "File_All") == 0)
    {
        file_all(home_dir);
        fprintf(logFile_fp, "[%s] 自定义文件归类\n", nowTime);
    }
    else if (strcasecmp(COM[1], "StopInstall") == 0)
    {
        STOPInstall(home_dir, COM[2]);
        fprintf(logFile_fp, "[%s] %s阻止安装\n", nowTime, COM[2]);
    }
    else if (strcasecmp(COM[1], "StopStorage") == 0)
    {
        STOPStorage(home_dir, COM[2]);
        fprintf(logFile_fp, "[%s] %s内部储存固定\n", nowTime, COM[2]);
    }
    else if (strcasecmp(COM[1], "F2fs_GC") == 0)
    {
        f2fs_GC(home_dir);
        fprintf(logFile_fp, "[%s] F2FS GC\n", nowTime);
    }
    else if (strcasecmp(COM[1], "Dexoat_1") == 0)
    {
        Dexoat_SYSTEM_DEXOAT(home_dir);
        fprintf(logFile_fp, "[%s] 触发系统Dexoat\n", nowTime);
    }
    else if (strcasecmp(COM[1], "Dexoat_2") == 0)
    {
        Dexoat_FAST_DEXOAT(home_dir, COM[2]);
        fprintf(logFile_fp, "[%s] 自定义模式Dexoat\n", nowTime);
    }
    else if (strcasecmp(COM[1], "Freezer") == 0)
    {
        freezer(home_dir);
    }
    else
    {
        printf(" » 传入参数错误！\n");
    }
    
    //ON the Selinux
    if (OffSelinux == 1) system("setenforce 1");
    if (logFile_i) fclose(logFile_fp);
    
    return 0;
}

// 一个通用函数，用途：执行脚本
static int Run(char * cmd, char * str, char * str2)
{
    if (access(cmd, F_OK))
    {
        return 1;
    }
    
    char * args[5] = {NULL};
    
    if (strstr(cmd, ".sh"))
    {
        args[0] = "bash";
        args[1] = cmd;
        args[2] = str;
        args[3] = str2;
        args[4] = NULL;
    }
    else
    {
        args[0] = cmd;
        args[1] = str;
        args[2] = str2;
        args[3] = NULL;
    }
    
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

// 清理第三方软件缓存
static int clear_cache(char * home_dir)
{
    //定义命令 & Log文件
    char command[strlen(home_dir) + 32];
    command[0] = '\0';
    snprintf(command, sizeof(command), "%s/wipe_cache/data_cache", home_dir);
    return Run(command, "", "");
}

// 清理系统软件缓存
static int clear_scache(char * home_dir)
{
    char command[strlen(home_dir) + 32];
    command[0] = '\0';
    snprintf(command, sizeof(command), "%s/wipe_cache/system_cache", home_dir);
    return Run(command, "", "");
}

// 运行处理自定义规则
static int list_dir(char * home_dir)
{
    char command[strlen(home_dir) + 32];
    command[0] = '\0';
    snprintf(command, sizeof(command), "%s/wipe_cache/wipe_list_dir.sh", home_dir);
    return Run(command, "", "");
}

// 清理储存目录
static int all_dir(char * home_dir)
{
    char command[strlen(home_dir) + 32];
    command[0] = '\0';
    snprintf(command, sizeof(command), "%s/wipe_cache/wipe_all_dir.sh", home_dir);
    if (access(command, F_OK))
    {
        return 1;
    }
    return Run(command, "", "");
}

// 运行规则清理、清理储存目录
static int dir_file(char * home_dir)
{
    list_dir(home_dir);
    all_dir(home_dir);
    return 0;
}

// 根据prop配置，清理全部文件（仅用于一键/自动清理 该选项打开状态
static int clear_tar(char * home_dir, char * work_dir)
{
    int clearall = 0;
    char * key = NULL;
    char settingsFile[strlen(work_dir) + 32], temp[64] = "";
    settingsFile[0] = '\0';
    snprintf(settingsFile, sizeof(settingsFile), "%s/settings.prop", work_dir);
    FILE * settingsFile_fp = fopen(settingsFile, "r");
    if (settingsFile_fp)
    {
        while (fgets(temp, sizeof(temp), settingsFile_fp))
        {
            temp[strcspn(temp, "\n")] = 0;
            if (strstr(temp, "clearall="))
            {
                key = strtok(temp, "=");
                key = strtok(NULL, "=");
                if (strcmp(key, "1") == 0)
                {
                    clearall = 1;
                }
            }
        }
        fclose(settingsFile_fp);
    }
    
    if (clearall == 1)
    {
        char command_1[strlen(home_dir) + 32], command_2[strlen(home_dir) + 32];
        command_1[0] = '\0';
        command_2[0] = '\0';
        snprintf(command_1, sizeof(command_1), "%s/wipe_cache/ClearService1.sh", home_dir);
        snprintf(command_2, sizeof(command_2), "%s/wipe_cache/ClearService2.sh", home_dir);
        Run(command_1, "", "");
        Run(command_2, "", "");
    }
    
    return 0;
}

// 自定义格式文件清理
static int FileClear(char * home_dir, char * str)
{
    char command_1[strlen(home_dir) + 64], command_2[strlen(home_dir) + 64];
    command_1[0] = '\0';
    command_2[0] = '\0';
    snprintf(command_1, sizeof(command_1), "%s/wipe_cache/ClearService1.sh", home_dir);
    snprintf(command_2, sizeof(command_2), "%s/wipe_cache/ClearService2.sh", home_dir);
    Run(command_1, str, "");
    Run(command_2, str, "");
    return 0;
}

// 自定义软件 规则清理
static int ClearApp(char * home_dir, char * str)
{
    char command[strlen(home_dir) + 64];
    command[0] = '\0';
    snprintf(command, sizeof(command), "%s/wipe_cache/AppClean", home_dir);
    return Run(command, str, "");
}

// 自定义格式文件归类
static int file_all(char * home_dir)
{
    char command_1[strlen(home_dir) + 32], command_2[strlen(home_dir) + 32];
    command_1[0] = '\0';
    command_2[0] = '\0';
    snprintf(command_1, sizeof(command_1), "%s/wipe_cache/file_1.sh", home_dir);
    snprintf(command_2, sizeof(command_2), "%s/wipe_cache/file_2.sh", home_dir);
    Run(command_1, "", "");
    Run(command_2, "", "");
    return 0;
}

// 根据prop决定是否运行文件归类（仅用于一键/自动清理 该选项打开状态
static int file_all2(char * home_dir, char * work_dir)
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
        char command_1[strlen(home_dir) + 32], command_2[strlen(home_dir) + 32];
        command_1[0] = '\0';
        command_2[0] = '\0';
        snprintf(command_1, sizeof(command_1), "%s/wipe_cache/file_1.sh", home_dir);
        snprintf(command_2, sizeof(command_2), "%s/wipe_cache/file_2.sh", home_dir);
        Run(command_1, "", "");
        Run(command_2, "", "");
    }
    
    return 0;
}

// 阻止安装
static int STOPInstall(char * home_dir, char * str)
{
    char command[strlen(home_dir) + 64];
    command[0] = '\0';
    snprintf(command, sizeof(command), "%s/wipe_cache/StopInstall.sh", home_dir);
    return Run(command, str, "");
}

// 内部储存固定
static int STOPStorage(char * home_dir, char * str)
{
    char command[strlen(home_dir) + 64];
    command[0] = '\0';
    snprintf(command, sizeof(command), "%s/wipe_cache/StopStorage.sh", home_dir);
    return Run(command, str, "");
}

// 磁盘GC
static int f2fs_GC(char * home_dir)
{
    char command[strlen(home_dir) + 32];
    command[0] = '\0';
    snprintf(command, sizeof(command), "%s/wipe_cache/f2fs_GC.sh", home_dir);
    return Run(command, "F2FS_GC", "");
}

// 快速GC
static int fast_GC(char * home_dir)
{
    char command[strlen(home_dir) + 32];
    command[0] = '\0';
    snprintf(command, sizeof(command), "%s/wipe_cache/f2fs_GC.sh", home_dir);
    return Run(command, "FAST_GC", "");
}

// Dexoat 优化1：触发系统Dexoat
static int Dexoat_SYSTEM_DEXOAT(char * home_dir)
{
    char command[strlen(home_dir) + 32];
    command[0] = '\0';
    snprintf(command, sizeof(command), "%s/wipe_cache/Dexoat.sh", home_dir);
    return Run(command, "SYSTEM_DEXOAT", "");
}

// Dexoat 优化2：自定义模式Dexoat
static int Dexoat_FAST_DEXOAT(char * home_dir, char * str)
{
    char command[strlen(home_dir) + 64];
    command[0] = '\0';
    snprintf(command, sizeof(command), "%s/wipe_cache/Dexoat.sh", home_dir);
    return Run(command, "FAST_DEXOAT", str);
}

// 其它优化，打开原生墓碑
static int freezer(char * home_dir)
{
    char command[strlen(home_dir) + 32];
    command[0] = '\0';
    snprintf(command, sizeof(command), "%s/wipe_cache/FreeZer.sh", home_dir);
    return Run(command, "", "");
}