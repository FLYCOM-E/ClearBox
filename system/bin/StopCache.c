// By ClearBox StopCache
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/wait.h>

#define DEBUG 0

static int Stopdata_app(char * dir, char * topApp, char * resetApp, char * workDir);

int main()
{
    uid_t nowuid = getuid();
    if (nowuid != 0)
    {
        printf(" » Please use root privileges!\n");
        return 1;
    }
    if (system("ClearBox -v >/dev/null 2>&1") != 0)
    {
        printf(" » 模块加载异常，请排查反馈！\n");
        return 1;
    }
    
    //work_dir定义
    char work_dir[64] = "";
    FILE * work_dir_fp = popen("ClearBox -w", "r");
    fgets(work_dir, sizeof(work_dir), work_dir_fp);
    work_dir[strcspn(work_dir, "\n")] = 0;
    pclose(work_dir_fp);
    
    //data_dir定义
    char data_dir[64] = "/data/data";
    
    //micro_dir定义
    char cardname[128] = "", micro_dir[256] = "";
    FILE * cardname_fp = popen("ls /mnt/expand/ | cut -f1 -d ' '", "r");
    fgets(cardname, sizeof(cardname), cardname_fp);
    cardname[strcspn(cardname, "\n")] = 0;
    snprintf(micro_dir, sizeof(micro_dir), "/mnt/expand/%s/user/0", cardname);
    pclose(cardname_fp);
    
    //定义储存文件
    char RunStart_File[64] = "";
    snprintf(RunStart_File, sizeof(RunStart_File), "%s/RunStart", work_dir);
    
    //定义待处理app临时储存变量
    char NowApp[5][64] = {0}, reset_app[64] = "";
    
    //提取RunStart储存值
    char str[64] = "";
    char * str_fp = NULL;
    FILE * RunStart_File_fp = fopen(RunStart_File, "r");
    
    if (RunStart_File_fp)
    {
        //循环遍历获取储存值
        while (fgets(str, sizeof(str), RunStart_File_fp) != NULL)
        {
            str[strcspn(str, "\n")] = 0;
            
            //if匹配，如没找到对应值或值为空则跳过（保持原空值）
            if (strstr(str, "1="))
            {
                str_fp = strtok(str, "=");
                str_fp = strtok(NULL, "=");
                if (str_fp)
                {
                    snprintf(NowApp[0], sizeof(NowApp[0]), "%s", str_fp);
                }
            }
            else if (strstr(str, "2="))
            {
                str_fp = strtok(str, "=");
                str_fp = strtok(NULL, "=");
                if (str_fp)
                {
                    snprintf(NowApp[1], sizeof(NowApp[1]), "%s", str_fp);
                }
            }
            else if (strstr(str, "3="))
            {
                str_fp = strtok(str, "=");
                str_fp = strtok(NULL, "=");
                if (str_fp)
                {
                    snprintf(NowApp[2], sizeof(NowApp[2]), "%s", str_fp);
                }
            }
            else if (strstr(str, "4="))
            {
                str_fp = strtok(str, "=");
                str_fp = strtok(NULL, "=");
                if (str_fp)
                {
                    snprintf(NowApp[3], sizeof(NowApp[3]), "%s", str_fp);
                }
            }
            else if (strstr(str, "5="))
            {
                str_fp = strtok(str, "=");
                str_fp = strtok(NULL, "=");
                if (str_fp)
                {
                    snprintf(NowApp[4], sizeof(NowApp[4]), "%s", str_fp);
                }
            }
            else if (strstr(str, "reset="))
            {
                str_fp = strtok(str, "=");
                str_fp = strtok(NULL, "=");
                if (str_fp)
                {
                    snprintf(reset_app, sizeof(reset_app), "%s", str_fp);
                }
            }
        }
        fclose(RunStart_File_fp);
    }
    else
    {
        //Else说明文件可能未初始化，因此初始化文件
        FILE * RunStart_File_fp = fopen(RunStart_File, "w");
        if (RunStart_File_fp)
        {
            fprintf(RunStart_File_fp, "1=\n2=\n3=\n4=\n5=\nreset=");
            fclose(RunStart_File_fp);
        }
    }
    
    // 如果包名含“/”则丢弃
    for (int i = 0; i < 5; i++)
    {
        if (strstr(NowApp[i], "/") != NULL)
        {
            NowApp[i][0] = '\0';
        }
    }
    
    // 调试模式关闭则创建子进程脱离终端
    if (DEBUG == 0)
    {
        pid_t PID = fork();
        if (PID == -1)
        {
            printf(" » 进程启动失败！\n");
            return 1;
        }
        else if (PID != 0)
        {
            exit(0);
        }
        setsid();
        
        int fd = open("/dev/null", O_RDWR);
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        close(fd);
    }
    
    //为渐进式等待初始化变量
    int cycle_count = 0, cycle_time = 10, max_cycle_time = 30, maxGetError = 0;
    //Start the cycle
    for ( ; ; )
    {
        //这里检查渐进等待时间，超过30则保持不再增长
        if (cycle_time > max_cycle_time)
        {
            cycle_time = max_cycle_time;
        }
        
        //获取前台软件包名
        char NowPackageName[64] = "";
        FILE * NowPackageName_fp = popen("dumpsys window | grep mCurrentFocus | head -n 1 | cut -f 1 -d '/' | cut -f 5 -d ' ' | cut -f 1 -d ' '", "r");
        if (NowPackageName_fp == NULL)
        {
            printf("Get Top App Failed. Continue");
            maxGetError++;
            if (maxGetError == 10)
            {
                printf("Get Top App Error, Timeout...\n");
                pclose(NowPackageName_fp);
                return 1;
            }
            sleep(5);
            continue;
        }
        fgets(NowPackageName, sizeof(NowPackageName), NowPackageName_fp);
        NowPackageName[strcspn(NowPackageName, "\n")] = 0;
        pclose(NowPackageName_fp);
        maxGetError = 0;
    
        //检查屏幕状态是否关闭或为控制中心、电源菜单、空
        //NotificationShade是A12及以上控制中心新名称
        if (strcmp(NowPackageName, "") == 0 ||
           strstr(NowPackageName, "NotificationShade") ||
           strstr(NowPackageName, "StatusBar") ||
           strstr(NowPackageName, "ActionsDialog"))
        {
            //渐进式等待
            cycle_count++;
            if (cycle_count == 5)
            {
                cycle_time += 2;
                cycle_count = 0;
            }
            sleep(cycle_time);
            continue;
        }
        
        //检查上一次前台App是否与当前一致
        if (strcmp(NowPackageName, NowApp[0]) == 0)
        {
            //渐进式等待
            cycle_count++;
            if (cycle_count == 5)
            {
                cycle_time += 2;
                cycle_count = 0;
            }
            sleep(cycle_time);
            continue;
        }
        else
        {
            //更新当前包名记录
            FILE * RunStart_File_fp = fopen(RunStart_File, "w");
            if (RunStart_File_fp)
            {
                fprintf(RunStart_File_fp, "1=%s\n2=%s\n3=%s\n4=%s\n5=%s\nreset=%s", NowPackageName, NowApp[0], NowApp[1], NowApp[2], NowApp[3], NowApp[4]);
                fclose(RunStart_File_fp);
            }
            
            cycle_count = 0;
            cycle_time = 10;
        }
        
        //更新值
        snprintf(reset_app, sizeof(reset_app), "%s", NowApp[4]);
        snprintf(NowApp[4], sizeof(NowApp[4]), "%s", NowApp[3]);
        snprintf(NowApp[3], sizeof(NowApp[3]), "%s", NowApp[2]);
        snprintf(NowApp[2], sizeof(NowApp[2]), "%s", NowApp[1]);
        snprintf(NowApp[1], sizeof(NowApp[1]), "%s", NowApp[0]);
        snprintf(NowApp[0], sizeof(NowApp[0]), "%s", NowPackageName);
        
        //调用处理函数
        Stopdata_app(data_dir, NowApp[0], reset_app, work_dir);
        
        //如果存在拓展SD则处理
        if (access(micro_dir, F_OK) == 0)
        {
            Stopdata_app(micro_dir, NowApp[0], reset_app, work_dir);
        }
        
        sleep(cycle_time); //能运行到这里渐进时间已经重置了，只是统一标准等待时间
    }
    
    return 0;
}

//此函数用于StopApp缓存
static int Stopdata_app(char * dir, char * topApp, char * resetApp, char * workDir)
{
    char topAppDir[256] = "", resetAppDir[256] = "", whiteList[128] = "";
    snprintf(topAppDir, sizeof(topAppDir), "%s/%s/cache", dir, topApp);     //topApp缓存目录定义
    snprintf(resetAppDir, sizeof(resetAppDir), "%s/%s/cache", dir, resetApp); //resetApp缓存目录定义
    snprintf(whiteList, sizeof(whiteList), "%s/whitelist.prop", workDir);       //定义WhiteList
    int inWhiteList = 0;
    
    //检查缓存目录是否真实存在
    if (access(topAppDir, F_OK) == 0 &&
       strstr(topAppDir, "../") == NULL)
    {
        //检查是否位于白名单
        char package[64] = "";
        FILE * whiteList_fp = fopen(whiteList, "r");
        if (whiteList_fp)
        {
            while (fgets(package, sizeof(package), whiteList_fp))
            {
                package[strcspn(package, "\n")] = 0;
                
                if (strcmp(topApp, package) == 0)
                {
                    //如果在白名单找到该包名则将inWhiteList置1
                    inWhiteList = 1;
                    break;
                }
            }
            fclose(whiteList_fp);
        }
        
        //检查inWhiteList = 0时执行缓存阻止
        if (inWhiteList == 0)
        {
            pid_t newPid = fork();
            if (newPid == -1)
            {
                printf(" » Stop: %s : Fork Err\n", topApp);
                goto reset; // 失败后直接跳过Stop部分
            }
            if (newPid == 0)
            {
                execlp("chattr", "StopCacheChattr", "-R", "+i", topAppDir, NULL);
                _exit(127);
            }
            else
            {
                int end = 0;
                if (waitpid(newPid, &end, 0) == -1)
                {
                    printf(" » Stop: %s : Wait Err\n", topApp);
                    goto reset; // 失败后直接跳过Stop部分
                }
                
                if (WIFEXITED(end) && WEXITSTATUS(end) == 0)
                {
                    printf(" » Stop: %s\n", topApp);
                }
                else
                {
                    printf(" » Stop: %s Error\n", topApp);
                }
            }
        }
    }
    
    reset:
    
    if (strcmp(topApp, resetApp) == 0)
    {
        return 0;
    }
    
    //检查缓存目录是否真实存在
    if (access(resetAppDir, F_OK) == 0 &&
       strstr(resetAppDir, "../") == NULL)
    {
        pid_t newPid = fork();
        if (newPid == -1)
        {
            printf(" » Reset: %s : Fork Err\n", resetApp);
            return 1;
        }
        if (newPid == 0)
        {
            execlp("chattr", "StopCacheChattr", "-R", "-i", resetAppDir, NULL);
            _exit(127);
        }
        else
        {
            int end = 0;
            if (waitpid(newPid, &end, 0) == -1)
            {
                printf(" » Reset: %s Error : Wait Err\n", resetApp);
                return 1;
            }
            
            if (WIFEXITED(end) && WEXITSTATUS(end) == 0)
            {
                printf(" » Reset: %s\n", resetApp);
            }
            else
            {
                printf(" » Reset: %s Error\n", resetApp);
            }
        }
    }
    return 0;
}
