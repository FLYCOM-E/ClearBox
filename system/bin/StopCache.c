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
        return 0;
    }
    
    //work_dir定义
    //whitelist="$work_dir/whitelist.prop"
    char work_dir[256] = "";
    FILE * work_dir_fp = popen("ClearBox -w", "r");
    fgets(work_dir, sizeof(work_dir), work_dir_fp);
    work_dir[strcspn(work_dir, "\n")] = 0;
    pclose(work_dir_fp);
    
    //data_dir定义
    char data_dir[64] = "/data/data";
    
    //micro_dir定义
    char cardname[64] = "";
    char micro_dir[128] = "";
    FILE * cardname_fp = popen("ls /mnt/expand/", "r");
    fgets(cardname, sizeof(cardname), cardname_fp);
    cardname[strcspn(cardname, "\n")] = 0;
    snprintf(micro_dir, sizeof(micro_dir), "/mnt/expand/%s/user/0", cardname);
    pclose(cardname_fp);
    
    //定义待处理app临时储存变量
    char NowApp1[64] = "", HowApp2[64] = "", reset_app[64] = "";
    
    //定义RunStart初始化
    char setting_command[256] = "";
    char checkSettings[256] = "grep '1=' ";
    char Setting[256] = "echo 1=\n2=\nreset= > ";
    char File[12] = "/RunStart";
    char Null[12] = " >>/dev/null";
    int Settings = 0;
    
    //拼接初始化检查命令
    strcat(checkSettings, work_dir);
    strcat(checkSettings, File);
    strcat(checkSettings, Null);
    
    //拼接初始化命令
    strcat(setting_command, Setting);
    strcat(setting_command, work_dir);
    strcat(setting_command, File);
    
    //检查/初始化
    Settings = system(checkSettings);
    if (Settings != 0)
    {
        system(setting_command);
    }
    
    //定义更新文件
    char RunStart_File[128] = "";
    strcat(RunStart_File, work_dir);
    strcat(RunStart_File, File);
    
    //定义get命令，获取当前值
    char reset_app_R[256] = "", NowApp2_R[256] = "", NowApp1_R[256] = "";
    snprintf(reset_app_R, sizeof(reset_app_R), "grep ^'reset=' '%s' | cut -f2 -d '='", RunStart_File);
    snprintf(NowApp2_R, sizeof(NowApp2_R), "grep ^'2=' '%s' | cut -f2 -d '='", RunStart_File);
    snprintf(NowApp1_R, sizeof(NowApp1_R), "grep ^'1=' '%s' | cut -f2 -d '='", RunStart_File);
    //运行并提取值
    //char NowApp1[64] = "", HowApp2[64] = "", reset_app[64] = "";
    FILE * NowApp1_fp = popen(NowApp1_R, "r");
    fgets(NowApp1, sizeof(NowApp1), NowApp1_fp);
    NowApp1[strcspn(NowApp1, "\n")] = 0;
    pclose(NowApp1_fp);
    FILE * HowApp2_fp = popen(NowApp2_R, "r");
    fgets(HowApp2, sizeof(HowApp2), HowApp2_fp);
    HowApp2[strcspn(HowApp2, "\n")] = 0;
    pclose(HowApp2_fp);
    FILE * reset_app_fp = popen(reset_app_R, "r");
    fgets(reset_app, sizeof(reset_app), reset_app_fp);
    reset_app[strcspn(reset_app, "\n")] = 0;
    pclose(reset_app_fp);
    
    //Start the cycle
    for ( ; ; )
    {
        //获取前台软件包名
        char NowPackageName[64] = "";
        FILE * NowPackageName_fp = popen("dumpsys window | grep mCurrentFocus | head -n 1 | cut -f 1 -d '/' | cut -f 5 -d ' ' | cut -f 1 -d ' '", "r");
        fgets(NowPackageName, sizeof(NowPackageName), NowPackageName_fp);
        NowPackageName[strcspn(NowPackageName, "\n")] = 0;
        pclose(NowPackageName_fp);
        
        //定义屏幕状态检查命令
        int a = 0;
        char checkhw[256] = "";
        snprintf(checkhw, sizeof(checkhw), "echo '%s' | grep StatusBar >>/dev/null 2>&1", NowPackageName);
        //定义前台变化检查命令
        int b = 0;
        char checkUpdate[256] = "";
        snprintf(checkUpdate, sizeof(checkUpdate), "grep ^1=%s %s/RunStart >>/dev/null 2>&1", NowApp1, work_dir);
        //Run
        a = system(checkhw);
        b = system(checkUpdate);
        //check
        if (a == 0)
        {
            printf("HW is Off\n");
            sleep(30);
            continue;
        }
        if (b == 0)
        {
            printf("App not Cycle\n");
            sleep(10);
            continue;
        }
        else
        {
            //更新当前包名记录
            char reset_app_C[256] = "", NowApp2_C[256] = "", NowApp1_C[256] = "";
            snprintf(reset_app_C, sizeof(reset_app_C), "sed -i 's|reset=%s|reset=%s|g' '%s'", reset_app, HowApp2, RunStart_File);
            snprintf(NowApp2_C, sizeof(NowApp2_C), "sed -i 's|2=%s|2=%s|g' '%s'", HowApp2, NowApp1, RunStart_File);
            snprintf(NowApp1_C, sizeof(NowApp1_C), "sed -i 's|1=%s|1=%s|g' '%s'", NowApp1, NowPackageName, RunStart_File);
            system(reset_app_C);
            system(NowApp2_C);
            system(NowApp1_C);
        }
        //定义get命令
        char reset_app_R[256] = "", NowApp2_R[256] = "", NowApp1_R[256] = "";
        snprintf(reset_app_R, sizeof(reset_app_R), "grep ^'reset=' '%s' | cut -f2 -d '='", RunStart_File);
        snprintf(NowApp2_R, sizeof(NowApp2_R), "grep ^'2=' '%s' | cut -f2 -d '='", RunStart_File);
        snprintf(NowApp1_R, sizeof(NowApp1_R), "grep ^'1=' '%s' | cut -f2 -d '='", RunStart_File);
        //运行并提取值
        //char NowApp1[64] = "", HowApp2[64] = "", reset_app[64] = "";
        FILE * NowApp1_fp = popen(NowApp1_R, "r");
        fgets(NowApp1, sizeof(NowApp1), NowApp1_fp);
        NowApp1[strcspn(NowApp1, "\n")] = 0;
        pclose(NowApp1_fp);
        
        FILE * HowApp2_fp = popen(NowApp2_R, "r");
        fgets(HowApp2, sizeof(HowApp2), HowApp2_fp);
        HowApp2[strcspn(HowApp2, "\n")] = 0;
        pclose(HowApp2_fp);
        
        FILE * reset_app_fp = popen(reset_app_R, "r");
        fgets(reset_app, sizeof(reset_app), reset_app_fp);
        reset_app[strcspn(reset_app, "\n")] = 0;
        pclose(reset_app_fp);
        
        DIR * dir = NULL;
        dir = opendir(micro_dir);
        //StopCache
        char * Stopdata_app_A[3] = {data_dir, NowApp1, reset_app};
        Stopdata_app(3, Stopdata_app_A);
        if (dir != NULL)
        {
            char * Stopdata_app_B[3] = {micro_dir, NowApp1, reset_app};
            Stopdata_app(3, Stopdata_app_B);
            closedir(dir);
        }
        
        sleep(10);
        
    }
    
    return 0;
}

//此函数用于StopApp缓存
int Stopdata_app(int COMI, char * COM[])
{
    //App目录定义
    char NowApp1_dir[128] = "", reset_app_dir[128] = "";
    snprintf(NowApp1_dir, sizeof(NowApp1_dir), "%s/%s", COM[0], COM[1]);
    snprintf(reset_app_dir, sizeof(reset_app_dir), "%s/%s", COM[0], COM[2]);
    
    //命令定义
    char StopNowApp1[128] = "", Reset_app[128] = "";
    snprintf(StopNowApp1, sizeof(StopNowApp1), "chattr -R +i %s/cache", NowApp1_dir);
    snprintf(Reset_app, sizeof(Reset_app), "chattr -R -i %s/cache", reset_app_dir);
    
    DIR * dir1 = NULL;
    DIR * dir2 = NULL;
    
    dir1 = opendir(NowApp1_dir);
    if (dir1 != NULL)
    {
        system(StopNowApp1);
        printf("Stop: %s\n", COM[1]);
        closedir(dir1);
    }
    
    dir2 = opendir(reset_app_dir);
    if (dir2 != NULL)
    {
        system(Reset_app);
        printf("Reset: %s\n", COM[2]);
        closedir(dir2);
    }
    
    return 0;
}
