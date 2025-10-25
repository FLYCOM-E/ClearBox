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
    char cardname[64] = "";
    char micro_dir[128] = "";
    FILE * cardname_fp = popen("ls /mnt/expand/ | cut -f1 -d ' '", "r");
    fgets(cardname, sizeof(cardname), cardname_fp);
    cardname[strcspn(cardname, "\n")] = 0;
    snprintf(micro_dir, sizeof(micro_dir), "/mnt/expand/%s/user/0", cardname);
    pclose(cardname_fp);
    
    //定义待处理app临时储存变量
    char NowApp1[64] = "", NowApp2[64] = "", NowApp3[64] = "", NowApp4[64] = "", NowApp5[64] = "", reset_app[64] = "";
    
    //定义储存文件
    char RunStart_File[64] = "";
    snprintf(RunStart_File, sizeof(RunStart_File), "%s/RunStart", work_dir);
    
    //储存文件初始化
    char setting_command[128] = "";
    char checkSettings[128] = "";
    
    snprintf(setting_command, sizeof(setting_command), "echo -e '1=\n2=\n3=\n4=\n5=\nreset=' > '%s'", RunStart_File);
    snprintf(checkSettings, sizeof(checkSettings), "grep '1=' '%s' >>/dev/null 2>&1", RunStart_File);
    
    int var = system(checkSettings);
    if (var != 0)
    {
        system(setting_command);
    }
    
    //Get获取当前储存值
    char reset_app_R[128] = "", NowApp5_R[128] = "", NowApp4_R[128] = "", NowApp3_R[128] = "", NowApp2_R[128] = "", NowApp1_R[128] = "";
    snprintf(reset_app_R, sizeof(reset_app_R), "grep ^'reset=' '%s' | cut -f2 -d '='", RunStart_File);
    snprintf(NowApp5_R, sizeof(NowApp5_R), "grep ^'5=' '%s' | cut -f2 -d '='", RunStart_File);
    snprintf(NowApp4_R, sizeof(NowApp4_R), "grep ^'4=' '%s' | cut -f2 -d '='", RunStart_File);
    snprintf(NowApp3_R, sizeof(NowApp3_R), "grep ^'3=' '%s' | cut -f2 -d '='", RunStart_File);
    snprintf(NowApp2_R, sizeof(NowApp2_R), "grep ^'2=' '%s' | cut -f2 -d '='", RunStart_File);
    snprintf(NowApp1_R, sizeof(NowApp1_R), "grep ^'1=' '%s' | cut -f2 -d '='", RunStart_File);
    
    FILE * NowApp1_fp = popen(NowApp1_R, "r");
    fgets(NowApp1, sizeof(NowApp1), NowApp1_fp);
    pclose(NowApp1_fp);
    NowApp1[strcspn(NowApp1, "\n")] = 0;
    
    FILE * NowApp2_fp = popen(NowApp2_R, "r");
    fgets(NowApp2, sizeof(NowApp2), NowApp2_fp);
    pclose(NowApp2_fp);
    NowApp2[strcspn(NowApp2, "\n")] = 0;
    
    FILE * NowApp3_fp = popen(NowApp3_R, "r");
    fgets(NowApp3, sizeof(NowApp3), NowApp3_fp);
    pclose(NowApp3_fp);
    NowApp3[strcspn(NowApp3, "\n")] = 0;
    
    FILE * NowApp4_fp = popen(NowApp4_R, "r");
    fgets(NowApp4, sizeof(NowApp4), NowApp4_fp);
    pclose(NowApp4_fp);
    NowApp4[strcspn(NowApp4, "\n")] = 0;
    
    FILE * NowApp5_fp = popen(NowApp5_R, "r");
    fgets(NowApp5, sizeof(NowApp5), NowApp5_fp);
    pclose(NowApp5_fp);
    NowApp5[strcspn(NowApp5, "\n")] = 0;
    
    FILE * reset_app_fp = popen(reset_app_R, "r");
    fgets(reset_app, sizeof(reset_app), reset_app_fp);
    pclose(reset_app_fp);
    reset_app[strcspn(reset_app, "\n")] = 0;
    
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
        int a = 0;
        char checkhw[128] = "";
        snprintf(checkhw, sizeof(checkhw), "echo '%s' | grep StatusBar >>/dev/null 2>&1", NowPackageName);
        
        a = system(checkhw);
        if (a == 0)
        {
            printf(" » Screen Is Off\n");
            sleep(30);
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
            char reset_app_C[128] = "", NowApp5_C[128] = "", NowApp4_C[128] = "", NowApp3_C[128] = "", NowApp2_C[128] = "", NowApp1_C[128] = "";
            snprintf(reset_app_C, sizeof(reset_app_C), "sed -i 's|reset=%s|reset=%s|g' '%s'", reset_app, NowApp5, RunStart_File);
            snprintf(NowApp5_C, sizeof(NowApp5_C), "sed -i 's|5=%s|5=%s|g' '%s'", NowApp5, NowApp4, RunStart_File);
            snprintf(NowApp4_C, sizeof(NowApp4_C), "sed -i 's|4=%s|4=%s|g' '%s'", NowApp4, NowApp3, RunStart_File);
            snprintf(NowApp3_C, sizeof(NowApp3_C), "sed -i 's|3=%s|3=%s|g' '%s'", NowApp3, NowApp2, RunStart_File);
            snprintf(NowApp2_C, sizeof(NowApp2_C), "sed -i 's|2=%s|2=%s|g' '%s'", NowApp2, NowApp1, RunStart_File);
            snprintf(NowApp1_C, sizeof(NowApp1_C), "sed -i 's|1=%s|1=%s|g' '%s'", NowApp1, NowPackageName, RunStart_File);
            
            system(reset_app_C);
            system(NowApp5_C);
            system(NowApp4_C);
            system(NowApp3_C);
            system(NowApp2_C);
            system(NowApp1_C);
        }
        
        //Get获取当前储存值
        char reset_app_R[128] = "", NowApp5_R[128] = "", NowApp4_R[128] = "", NowApp3_R[128] = "", NowApp2_R[128] = "", NowApp1_R[128] = "";
        snprintf(reset_app_R, sizeof(reset_app_R), "grep ^'reset=' '%s' | cut -f2 -d '='", RunStart_File);
        snprintf(NowApp5_R, sizeof(NowApp5_R), "grep ^'5=' '%s' | cut -f2 -d '='", RunStart_File);
        snprintf(NowApp4_R, sizeof(NowApp4_R), "grep ^'4=' '%s' | cut -f2 -d '='", RunStart_File);
        snprintf(NowApp3_R, sizeof(NowApp3_R), "grep ^'3=' '%s' | cut -f2 -d '='", RunStart_File);
        snprintf(NowApp2_R, sizeof(NowApp2_R), "grep ^'2=' '%s' | cut -f2 -d '='", RunStart_File);
        snprintf(NowApp1_R, sizeof(NowApp1_R), "grep ^'1=' '%s' | cut -f2 -d '='", RunStart_File);
        
        FILE * NowApp1_fp = popen(NowApp1_R, "r");
        fgets(NowApp1, sizeof(NowApp1), NowApp1_fp);
        pclose(NowApp1_fp);
        NowApp1[strcspn(NowApp1, "\n")] = 0;
        
        FILE * NowApp2_fp = popen(NowApp2_R, "r");
        fgets(NowApp2, sizeof(NowApp2), NowApp2_fp);
        pclose(NowApp2_fp);
        NowApp2[strcspn(NowApp2, "\n")] = 0;
        
        FILE * NowApp3_fp = popen(NowApp3_R, "r");
        fgets(NowApp3, sizeof(NowApp3), NowApp3_fp);
        pclose(NowApp3_fp);
        NowApp3[strcspn(NowApp3, "\n")] = 0;
        
        FILE * NowApp4_fp = popen(NowApp4_R, "r");
        fgets(NowApp4, sizeof(NowApp4), NowApp4_fp);
        pclose(NowApp4_fp);
        NowApp4[strcspn(NowApp4, "\n")] = 0;
        
        FILE * NowApp5_fp = popen(NowApp5_R, "r");
        fgets(NowApp5, sizeof(NowApp5), NowApp5_fp);
        pclose(NowApp5_fp);
        NowApp5[strcspn(NowApp5, "\n")] = 0;
        
        FILE * reset_app_fp = popen(reset_app_R, "r");
        fgets(reset_app, sizeof(reset_app), reset_app_fp);
        pclose(reset_app_fp);
        reset_app[strcspn(reset_app, "\n")] = 0;
        
        //调用处理函数
        char * Stopdata_app_A[4] = {data_dir, NowApp1, reset_app, work_dir};
        Stopdata_app(3, Stopdata_app_A);
        
        //如果存在拓展SD则处理
        DIR * dir = opendir(micro_dir);
        if (dir != NULL)
        {
            char * Stopdata_app_B[4] = {micro_dir, NowApp1, reset_app, work_dir};
            Stopdata_app(3, Stopdata_app_B);
            closedir(dir);
        }
        
        //等待10s
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
        printf(" » Check Err\n");
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
    if (dir1 != NULL)
    {
        int i = system(CheckWhitelist);
        if (i != 0)
        {
            system(StopNowApp1);
            printf(" » Stop: %s\n", COM[1]);
            closedir(dir1);
        }
    }
    
    DIR * dir2 = opendir(reset_app_dir);
    if (dir2 != NULL)
    {
        system(Reset_app);
        printf(" » Reset: %s\n", COM[2]);
        closedir(dir2);
    }
    
    return 0;
}
