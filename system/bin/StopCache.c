// By ClearBox StopCache
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

int Stopdata_app(int COMI, char * COM[]);

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
    char RunStart_File[64] = "", checkSettings[128] = "";
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
        //Else说明文件打开失败可能未初始化，因此初始化文件
        FILE * RunStart_File_fp = fopen(RunStart_File, "w");
        if (RunStart_File_fp)
        {
            fprintf(RunStart_File_fp, "1=\n2=\n3=\n4=\n5=\nreset=");
            fclose(RunStart_File_fp);
        }
    }
    
    //Start the cycle
    for ( ; ; )
    {
        //获取前台软件包名
        char NowPackageName[64] = "";
        FILE * NowPackageName_fp = popen("dumpsys window | grep mCurrentFocus | head -n 1 | cut -f 1 -d '/' | cut -f 5 -d ' ' | cut -f 1 -d ' '", "r");
        fgets(NowPackageName, sizeof(NowPackageName), NowPackageName_fp);
        NowPackageName[strcspn(NowPackageName, "\n")] = 0;
        pclose(NowPackageName_fp);
    
        //检查屏幕状态是否关闭
        if (strstr(NowPackageName, "StatusBar"))
        {
            printf(" » Top App is SystemUI or Screen is Off\n");
            sleep(15);
            //这里15秒感觉短了点，但考虑如果用户只是下拉SystemUI就不能设置太长，以后考虑更新判断方式
            continue;
        }
        
        //检查上一次前台App是否与当前一致
        if (strcmp(NowPackageName, NowApp1) == 0)
        {
            printf(" » App Not Cycle\n");
            sleep(10);
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
        }
        
        usleep(50000);
        
        //提取更新RunStart储存值
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
        
        //调用处理函数
        char * Stopdata_app_A[4] = {data_dir, NowApp1, reset_app, work_dir};
        Stopdata_app(3, Stopdata_app_A);
        
        //如果存在拓展SD则处理
        DIR * microDir = opendir(micro_dir);
        if (microDir)
        {
            char * Stopdata_app_B[4] = {micro_dir, NowApp1, reset_app, work_dir};
            Stopdata_app(3, Stopdata_app_B);
            closedir(microDir);
        }
        
        sleep(10);
    }
    
    return 0;
}

//此函数用于StopApp缓存
int Stopdata_app(int COMI, char * COM[])
{
    //检查参数数量
    if (COMI != 3)
    {
        printf(" » Err\n");
    }
    
    //App目录定义
    char NowApp1_dir[128] = "", reset_app_dir[128] = "";
    snprintf(NowApp1_dir, sizeof(NowApp1_dir), "%s/%s", COM[0], COM[1]);
    snprintf(reset_app_dir, sizeof(reset_app_dir), "%s/%s", COM[0], COM[2]);
    
    //命令定义
    char StopNowApp1[128] = "", Reset_app[128] = "", CheckWhitelist[128] = "";
    snprintf(StopNowApp1, sizeof(StopNowApp1), "chattr -R +i %s/cache", NowApp1_dir);
    snprintf(Reset_app, sizeof(Reset_app), "chattr -R -i %s/cache", reset_app_dir);
    snprintf(CheckWhitelist, sizeof(CheckWhitelist), "grep %s %s/whitelist.prop >>/dev/null 2>&1", COM[1], COM[3]);
    
    DIR * dir1 = opendir(NowApp1_dir);
    if (dir1)
    {
        int i = system(CheckWhitelist);
        if (i != 0)
        {
            system(StopNowApp1);
            printf(" » Stop: %s\n", COM[1]);
        }
        closedir(dir1);
    }
    
    DIR * dir2 = opendir(reset_app_dir);
    if (dir2)
    {
        system(Reset_app);
        printf(" » Reset: %s\n", COM[2]);
        closedir(dir2);
    }
    
    return 0;
}
