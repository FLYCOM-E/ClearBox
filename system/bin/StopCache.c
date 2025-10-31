// By ClearBox StopCache
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

static int Stopdata_app(char * dir, char * topApp, char * resetApp, char * workDir);

int main()
{
    uid_t nowuid = getuid();
    if (nowuid != 0)
    {
        printf(" » Please use root privileges!\n");
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
    char cardname[64] = "", micro_dir[128] = "";
    FILE * cardname_fp = popen("ls /mnt/expand/ | cut -f1 -d ' '", "r");
    fgets(cardname, sizeof(cardname), cardname_fp);
    cardname[strcspn(cardname, "\n")] = 0;
    snprintf(micro_dir, sizeof(micro_dir), "/mnt/expand/%s/user/0", cardname);
    pclose(cardname_fp);
    
    //定义储存文件
    char RunStart_File[64] = "";
    snprintf(RunStart_File, sizeof(RunStart_File), "%s/RunStart", work_dir);
    
    //定义待处理app临时储存变量
    char NowApp1[64] = "", NowApp2[64] = "", NowApp3[64] = "", NowApp4[64] = "", NowApp5[64] = "", reset_app[64] = "";
    
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
                    strcpy(NowApp1, str_fp);
                }
            }
            else if (strstr(str, "2="))
            {
                str_fp = strtok(str, "=");
                str_fp = strtok(NULL, "=");
                if (str_fp)
                {
                    strcpy(NowApp2, str_fp);
                }
            }
            else if (strstr(str, "3="))
            {
                str_fp = strtok(str, "=");
                str_fp = strtok(NULL, "=");
                if (str_fp)
                {
                    strcpy(NowApp3, str_fp);
                }
            }
            else if (strstr(str, "4="))
            {
                str_fp = strtok(str, "=");
                str_fp = strtok(NULL, "=");
                if (str_fp)
                {
                    strcpy(NowApp4, str_fp);
                }
            }
            else if (strstr(str, "5="))
            {
                str_fp = strtok(str, "=");
                str_fp = strtok(NULL, "=");
                if (str_fp)
                {
                    strcpy(NowApp5, str_fp);
                }
            }
            else if (strstr(str, "reset="))
            {
                str_fp = strtok(str, "=");
                str_fp = strtok(NULL, "=");
                if (str_fp)
                {
                    strcpy(reset_app, str_fp);
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
    
    //为渐进式等待初始化变量
    int cycle_count = 0, cycle_time = 10, max_cycle_time = 30;
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
        fgets(NowPackageName, sizeof(NowPackageName), NowPackageName_fp);
        NowPackageName[strcspn(NowPackageName, "\n")] = 0;
        pclose(NowPackageName_fp);
    
        //检查屏幕状态是否关闭或为控制中心、电源菜单
        //NotificationShade是A12及以上控制中心新名称
        if (strstr(NowPackageName, "NotificationShade") ||
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
        if (strcmp(NowPackageName, NowApp1) == 0)
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
                fprintf(RunStart_File_fp, "1=%s\n2=%s\n3=%s\n4=%s\n5=%s\nreset=%s", NowPackageName, NowApp1, NowApp2, NowApp3, NowApp4, NowApp5);
                fclose(RunStart_File_fp);
            }
            
            cycle_count = 0;
            cycle_time = 10;
        }
        
        //更新值
        snprintf(reset_app, sizeof(reset_app), "%s", NowApp5);
        snprintf(NowApp5, sizeof(NowApp5), "%s", NowApp4);
        snprintf(NowApp4, sizeof(NowApp4), "%s", NowApp3);
        snprintf(NowApp3, sizeof(NowApp3), "%s", NowApp2);
        snprintf(NowApp2, sizeof(NowApp2), "%s", NowApp1);
        snprintf(NowApp1, sizeof(NowApp1), "%s", NowPackageName);
        
        //调用处理函数
        Stopdata_app(data_dir, NowApp1, reset_app, work_dir);
        
        //如果存在拓展SD则处理
        if (access(micro_dir, F_OK) == 0)
        {
            Stopdata_app(micro_dir, NowApp1, reset_app, work_dir);
        }
        
        sleep(cycle_time); //能运行到这里渐进时间已经重置了，只是统一标准等待时间
    }
    
    return 0;
}

//此函数用于StopApp缓存
static int Stopdata_app(char * dir, char * topApp, char * resetApp, char * workDir)
{
    char topAppDir[128] = "", resetAppDir[128] = "", topAppCommand[128] = "", resetAppCommand[128] = "", whiteList[128] = "";
    snprintf(topAppDir, sizeof(topAppDir), "%s/%s", dir, topApp);                              //topApp缓存目录定义
    snprintf(resetAppDir, sizeof(resetAppDir), "%s/%s", dir, resetApp);                           //resetApp缓存目录定义
    snprintf(topAppCommand, sizeof(topAppCommand), "chattr -R +i %s/cache", topAppDir);    //topApp缓存阻止命令
    snprintf(resetAppCommand, sizeof(resetAppCommand), "chattr -R -i %s/cache", resetAppDir); //resetApp缓存阻止命令
    snprintf(whiteList, sizeof(whiteList), "%s/whitelist.prop", workDir);                           //定义WhiteList
    int inWhiteList = 0;
    
    //检查缓存目录是否真实存在
    if (access(topAppDir, F_OK) == 0)
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
                }
            }
            fclose(whiteList_fp);
        }
        
        //检查inWhiteList = 0时执行缓存阻止
        if (inWhiteList == 0)
        {
            int i = system(topAppCommand);
            if (i == 0)
            {
                printf(" » Stop: %s\n", topApp);
            }
            else
            {
                printf(" » Stop: %s Error\n", topApp);
            }
        }
    }
    
    //检查缓存目录是否真实存在
    if (access(resetAppDir, F_OK) == 0)
    {
        int i = system(resetAppCommand);
        if (i == 0)
        {
            printf(" » Reset: %s\n", resetApp);
        }
        else
        {
            printf(" » Reset: %s Error\n", resetApp);
        }
    }
    
    return 0;
}
