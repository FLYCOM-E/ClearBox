//By ClearBox
// 此Core来自ClearBox模块，用于根据传入参数调用清理脚本
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>

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
    int i = 0;
    
    uid_t nowUid = getuid();
    if (nowUid != 0)
    {
        printf(" » Please use root privileges!\n");
        return 1;
    }
    i = system("ClearBox -v >>/dev/null 2>&1");
    if (i != 0)
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
        fclose(home_dir_fp);
    }
    
    //work_dir定义
    char work_dir[64] = "";
    FILE * work_dir_fp = popen("ClearBox -w", "r");
    if (work_dir_fp)
    {
        fgets(work_dir, sizeof(work_dir), work_dir_fp);
        work_dir[strcspn(work_dir, "\n")] = 0;
        fclose(work_dir_fp);
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
            i = system("setenforce 0");
            if (i == 0)
            {
                OffSelinux = 1;
            }
        }
    }
    
    //获取当前时间（用于log）
    char nowTime[64] = "";
    time_t nowTimeC = time(NULL);
    struct tm * t = localtime(&nowTimeC);
    strftime(nowTime, sizeof(nowTime), "%m-%d %H:%M:%S", t);
    
    //定义并预打开Log文件
    char logFile[64] = "";
    snprintf(logFile, sizeof(logFile), "%s/运行日志.log", work_dir);
    FILE * logFile_fp = fopen(logFile, "a");
    if (logFile_fp == NULL)
    {
        logFile_fp = stderr;
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
        printf(" » 传入参数错误！");
    }
    
    //ON the Selinux
    if (OffSelinux == 1)
    {
        system("setenforce 1");
    }
    
    return 0;
}

// 清理第三方软件缓存
static int clear_cache(char * home_dir)
{
    //定义命令 & Log文件
    char command[64] = "";
    snprintf(command, sizeof(command), "%s/wipe_cache/data_cache", home_dir);
    if (access(command, F_OK))
    {
        return 1;
    }
    return system(command);
}

// 清理系统软件缓存
static int clear_scache(char * home_dir)
{
    char command[64] = "";
    snprintf(command, sizeof(command), "%s/wipe_cache/system_cache.sh", home_dir);
    if (access(command, F_OK))
    {
        return 1;
    }
    return system(command);
}

// 运行处理自定义规则
static int list_dir(char * home_dir)
{
    char command[64] = "";
    snprintf(command, sizeof(command), "%s/wipe_cache/wipe_list_dir.sh", home_dir);
    if (access(command, F_OK))
    {
        return 1;
    }
    return system(command);
}

// 清理储存目录
static int all_dir(char * home_dir)
{
    char command[64] = "";
    snprintf(command, sizeof(command), "%s/wipe_cache/wipe_all_dir.sh", home_dir);
    if (access(command, F_OK))
    {
        return 1;
    }
    return system(command);
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
    char settingsFile[64] = "", temp[64] = "";
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
        char command_1[64] = "", command_2[64] = "";
        snprintf(command_1, sizeof(command_1), "%s/wipe_cache/ClearService1.sh", home_dir);
        snprintf(command_2, sizeof(command_2), "%s/wipe_cache/ClearService2.sh", home_dir);
        system(command_1);
        system(command_2);
    }
    
    return 0;
}

// 自定义格式文件清理
static int FileClear(char * home_dir, char * str)
{
    char command_1[64] = "", command_2[64] = "";
    snprintf(command_1, sizeof(command_1), "%s/wipe_cache/ClearService1.sh %s", home_dir, str);
    snprintf(command_2, sizeof(command_2), "%s/wipe_cache/ClearService2.sh %s", home_dir, str);
    system(command_1);
    system(command_2);
    
    return 0;
}

// 自定义软件 规则清理
static int ClearApp(char * home_dir, char * str)
{
    char command[64] = "";
    snprintf(command, sizeof(command), "%s/wipe_cache/AppClean.sh %s", home_dir, str);
    return system(command);
}

// 自定义格式文件归类
static int file_all(char * home_dir)
{
    char command_1[64] = "", command_2[64] = "";
    snprintf(command_1, sizeof(command_1), "%s/wipe_cache/file_1.sh", home_dir);
    snprintf(command_1, sizeof(command_1), "%s/wipe_cache/file_2.sh", home_dir);
    system(command_1);
    system(command_2);
    
    return 0;
}

// 根据prop决定是否运行文件归类（仅用于一键/自动清理 该选项打开状态
static int file_all2(char * home_dir, char * work_dir)
{
    int fileall = 0;
    char * key = NULL;
    char temp[64] = "", settingsFile[64] = "";
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
        char command_1[64] = "", command_2[64] = "";
        snprintf(command_1, sizeof(command_1), "%s/wipe_cache/file_1.sh", home_dir);
        snprintf(command_2, sizeof(command_2), "%s/wipe_cache/file_2.sh", home_dir);
        system(command_1);
        system(command_2);
    }
    
    return 0;
}

// 阻止安装
static int STOPInstall(char * home_dir, char * str)
{
    char command[64] = "";
    snprintf(command, sizeof(command), "%s/wipe_cache/StopInstall.sh %s", home_dir, str);
    return system(command);
}

// 内部储存固定
static int STOPStorage(char * home_dir, char * str)
{
    char command[64] = "";
    snprintf(command, sizeof(command), "%s/wipe_cache/StopStorage.sh %s", home_dir, str);
    return system(command);
}

// 磁盘GC
static int f2fs_GC(char * home_dir)
{
    char command[64] = "";
    snprintf(command, sizeof(command), "%s/wipe_cache/f2fs_GC.sh F2FS_GC", home_dir);
    return system(command);
}

// 快速GC
static int fast_GC(char * home_dir)
{
    char command[64] = "";
    snprintf(command, sizeof(command), "%s/wipe_cache/f2fs_GC.sh FAST_GC", home_dir);
    return system(command);
}

// Dexoat 优化1：触发系统Dexoat
static int Dexoat_SYSTEM_DEXOAT(char * home_dir)
{
    char command[64] = "";
    snprintf(command, sizeof(command), "%s/wipe_cache/Dexoat.sh SYSTEM_DEXOAT", home_dir);
    return system(command);
}

// Dexoat 优化2：自定义模式Dexoat
static int Dexoat_FAST_DEXOAT(char * home_dir, char * str)
{
    char command[64] = "";
    snprintf(command, sizeof(command), "%s/wipe_cache/Dexoat.sh FAST_DEXOAT %s", home_dir, str);
    return system(command);
}

// 其它优化，打开原生墓碑
static int freezer(char * home_dir)
{
    char command[64] = "";
    snprintf(command, sizeof(command), "%s/wipe_cache/FreeZer.sh", home_dir);
    return system(command);
}
