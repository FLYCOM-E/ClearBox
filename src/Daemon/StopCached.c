// By ClearBox StopCache
#include "../INCLUDE/BashCore.h"

#define DATA_DIR "/data/data"
#define ROM_NAME "RunStart" //Max Size 30
#define WHITELIST_NAME "whitelist.prop"
#define GET_TOPAPP "dumpsys window | grep mCurrentFocus | head -n 1 | cut -f 1 -d '/' | cut -f 5 -d ' ' | cut -f 1 -d ' '"
#define MICRO_DATA_PATH "/mnt/expand/%s/user/0" //Max Size 100
#define GET_SD_ID "ls /mnt/expand/ | cut -f1 -d ' '"
#define LOGPRINT __android_log_print
#define SERVER_NAME "StopCached"

static int stopAppCache(char * dir, char * top_app, char * reset_app, char * work_dir, char * bin_dir);

int main(int argc, char * argv[])
{
    if (getuid() != 0)
    {
        fprintf(stderr, " » Please use root privileges!\n");
        return 1;
    }
    
    argc--;
    argv++;
    if (argc < 3)
    {
        fprintf(stderr, " » Cmd args Failed! \n");
        return 1;
    }
    
    char * work_dir = NULL;
    char * bin_dir = NULL;
    
    while (argc > 1)
    {
        if (strcmp(argv[0], "-w") == 0) //work_dir
        {
            if (strlen(argv[1]) > MAX_WORK_DIR_LEN)
            {
                fprintf(stderr, " » Config dir is Long! \n");
                return 1;
            }
            else if (access(argv[1], F_OK) != 0)
            {
                fprintf(stderr, " » Config dir Not Find! \n");
                return 1;
            }
            work_dir = argv[1];
            argc -= 2;
            argv += 2;
        }
        else if (strcmp(argv[0], "-b") == 0) //bin_dir
        {
            if (strlen(argv[1]) > MAX_BIN_DIR_LEN)
            {
                fprintf(stderr, " » Bin dir is Long! \n");
                return 1;
            }
            else if (access(argv[1], F_OK) != 0)
            {
                fprintf(stderr, " » Bin dir Not Find! \n");
                return 1;
            }
            bin_dir = argv[1];
            argc -= 2;
            argv += 2;
        }
        else
        {
            fprintf(stderr, " » Incorrect arguments provided!\n");
            return 1;
        }
    }
    if (work_dir == NULL)
    {
        fprintf(stderr, " » Config dir Not Find! \n");
        return 1;
    }
    if (bin_dir == NULL)
    {
        fprintf(stderr, " » Bin dir Not Find! \n");
        return 1;
    }
    
    //micro_dir定义
    char card_id[128] = "", micro_dir[256] = "";
    FILE * card_id_fp = popen(GET_SD_ID, "r");
    if (card_id_fp)
    {
        fgets(card_id, sizeof(card_id), card_id_fp);
        card_id[strcspn(card_id, "\n")] = 0;
        snprintf(micro_dir, sizeof(micro_dir), MICRO_DATA_PATH, card_id);
        pclose(card_id_fp);
    }
    
    //定义储存文件
    char rom_file[strlen(work_dir) + 32];
    snprintf(rom_file, sizeof(rom_file), "%s/%s", work_dir, ROM_NAME);
    
    //定义待处理app临时储存变量
    char top_app_list[5][MAX_PACKAGE] = {0}, reset_app[MAX_PACKAGE] = "";
    
    //提取RunStart储存值
    char tmp[16] = "";
    char rom_key_line[MAX_PACKAGE] = "";
    char * rom_key_line_p = NULL;
    FILE * rom_file_fp_r = fopen(rom_file, "r");
    if (rom_file_fp_r)
    {
        while (fgets(rom_key_line, sizeof(rom_key_line), rom_file_fp_r) != NULL)
        {
            rom_key_line[strcspn(rom_key_line, "\n")] = 0;
            
            //if匹配，如没找到对应值或值为空则跳过（保持原空值）
            int i = 0;
            while (i <= 4)
            {
                snprintf(tmp, sizeof(tmp), "%d=", i + 1);
                if (strstr(rom_key_line, tmp))
                {
                    rom_key_line_p = strtok(rom_key_line, "=");
                    rom_key_line_p = strtok(NULL, "=");
                    if (rom_key_line_p)
                    {
                        snprintf(top_app_list[i], sizeof(top_app_list[i]), "%s", rom_key_line_p);
                    }
                }
                i++;
            }
            if (strstr(rom_key_line, "reset="))
            {
                rom_key_line_p = strtok(rom_key_line, "=");
                rom_key_line_p = strtok(NULL, "=");
                if (rom_key_line_p)
                {
                    snprintf(reset_app, sizeof(reset_app), "%s", rom_key_line_p);
                }
            }
        }
        fclose(rom_file_fp_r);
    }
    
    // 如果包名含“ / ”则丢弃
    for (int i = 0; i < 5; i++)
    {
        if (strstr(top_app_list[i], "/") != NULL)
        {
            top_app_list[i][0] = '\0';
        }
    }
    
    // 创建子进程脱离终端
    pid_t PID = fork();
    if (PID == -1)
    {
        post(SERVER_NAME, "Start Server Failed!");
        return 1;
    }
    else if (PID != 0)
    {
        exit(0);
    }
    setsid();
    int std = open("/dev/null", O_RDWR);
    dup2(std, STDIN_FILENO);
    dup2(std, STDOUT_FILENO);
    dup2(std, STDERR_FILENO);
    close(std);
    post(SERVER_NAME, "Start Server Successful");
    
    /* 
    初始化变量
    全局错误值／空等待次数／渐进等待时间（变化）／最大渐进等待时间（秒）／获取前台App失败次数／最大获取前台App失败次数
    */
    int end = 0, cycle_count = 0, cycle_time = 10, max_cycle_time = 30, get_error = 0, max_get_error = 10;
    //Start the cycle
    for ( ; ; )
    {
        //这里检查渐进等待时间，超过30则保持不再增长
        if (cycle_time > max_cycle_time)
        {
            cycle_time = max_cycle_time;
        }
        
        //获取前台软件包名
        char top_app[MAX_PACKAGE] = "";
        FILE * top_app_fp = popen(GET_TOPAPP, "r");
        if (top_app_fp == NULL)
        {
            get_error++;
            if (get_error == max_get_error)
            {
                post(SERVER_NAME, "Get Top App Timeout... exit\n");
                end = 1;
                break;
            }
            sleep(5);
            LOGPRINT(ANDROID_LOG_WARN, SERVER_NAME, "Get Top App Failed. Continue\n");
            continue;
        }
        fgets(top_app, sizeof(top_app), top_app_fp);
        top_app[strcspn(top_app, "\n")] = 0;
        pclose(top_app_fp);
        get_error = 0;
        
        /*
        检查上一次前台App是否与当前一致
        检查屏幕状态是否关闭或为控制中心、电源菜单、空
        NotificationShade是A12及以上控制中心新名称
        */
        if (strcmp(top_app, "") == 0 ||
           strstr(top_app, "NotificationShade") ||
           strstr(top_app, "StatusBar") ||
           strstr(top_app, "ActionsDialog") ||
           strcmp(top_app, top_app_list[0]) == 0)
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
        
        // Reset cycle*
        cycle_count = 0;
        cycle_time = 10;
        
        //更新当前包名记录
        snprintf(reset_app, sizeof(reset_app), "%s", top_app_list[4]);
        snprintf(top_app_list[4], sizeof(top_app_list[4]), "%s", top_app_list[3]);
        snprintf(top_app_list[3], sizeof(top_app_list[3]), "%s", top_app_list[2]);
        snprintf(top_app_list[2], sizeof(top_app_list[2]), "%s", top_app_list[1]);
        snprintf(top_app_list[1], sizeof(top_app_list[1]), "%s", top_app_list[0]);
        snprintf(top_app_list[0], sizeof(top_app_list[0]), "%s", top_app);
        
        FILE * rom_file_fp_w = fopen(rom_file, "w");
        if (rom_file_fp_w)
        {
            fprintf(rom_file_fp_w, "1=%s\n2=%s\n3=%s\n4=%s\n5=%s\nreset=%s", top_app_list[0], top_app_list[1], top_app_list[2], top_app_list[3], top_app_list[4], reset_app);
            fclose(rom_file_fp_w);
        }
        
        // 这个逻辑目标为跳过不必要的缓存加解锁
        int i = 0;
        while (i < 5)
        {
            if (strcmp(reset_app, top_app_list[i]) == 0)
            {
                strcpy(reset_app, "null");
                break;
            }
            i++;
        }
        
        //调用处理函数
        stopAppCache(DATA_DIR, top_app_list[0], reset_app, work_dir, bin_dir);
        if (access(micro_dir, F_OK) == 0) //如果存在拓展SD则处理
        {
            stopAppCache(micro_dir, top_app_list[0], reset_app, work_dir, bin_dir);
        }
        
        sleep(cycle_time); //能运行到这里渐进时间已经重置了，只是统一标准等待时间
    }
    
    return end;
}

/*
缓存阻止实现
接收：
    char * dir 软件数据根目录
    char * top_app 前台App包名
    char * reset_app 待恢复App包名
    char * work_dir 配置目录
    char * bin_dir Bin目录
返回：
    int 成功返回0，失败返回-1
    
*/
static int stopAppCache(char * dir, char * top_app, char * reset_app, char * work_dir, char * bin_dir)
{
    char top_app_dir[strlen(dir) + strlen(top_app) + 16],
         reset_app_dir[strlen(dir) + strlen(reset_app) + 16],
         whitelist_file[strlen(work_dir) + strlen(WHITELIST_NAME) + 8],
         busybox_bin[strlen(bin_dir) + 32];
    snprintf(top_app_dir, sizeof(top_app_dir), "%s/%s/cache", dir, top_app);              //topApp缓存目录定义
    snprintf(reset_app_dir, sizeof(reset_app_dir), "%s/%s/cache", dir, reset_app);         //resetApp缓存目录定义
    snprintf(whitelist_file, sizeof(whitelist_file), "%s/%s", work_dir, WHITELIST_NAME);   //定义WhiteList
    snprintf(busybox_bin, sizeof(busybox_bin), "%s/busybox", bin_dir);                  //定义BusyBox
    int in_whitelist = 0;
    
    if (access(busybox_bin, F_OK) != 0)
    {
        LOGPRINT(ANDROID_LOG_WARN, SERVER_NAME, "busybox bin not Found! \n");
        return 1;
    }
    
    //检查缓存目录是否真实存在并过滤路径逃逸
    if (access(top_app_dir, F_OK) == 0 &&
       strstr(top_app_dir, "/../") == NULL)
    {
        //检查是否位于白名单
        char whitelist_line[MAX_PACKAGE] = "";
        FILE * whitelist_fp = fopen(whitelist_file, "r");
        if (whitelist_fp)
        {
            while (fgets(whitelist_line, sizeof(whitelist_line), whitelist_fp))
            {
                whitelist_line[strcspn(whitelist_line, "\n")] = 0;
                
                if (strcmp(top_app, whitelist_line) == 0)
                {
                    //如果在白名单找到该包名则将 in_whitelist 置 1
                    in_whitelist = 1;
                    break;
                }
            }
            fclose(whitelist_fp);
        }
        
        //检查 in_whitelist = 0 时执行缓存阻止
        if (in_whitelist == 0)
        {
            pid_t newPid = fork();
            if (newPid == -1)
            {
                LOGPRINT(ANDROID_LOG_WARN, SERVER_NAME, "Stop %s: Fork Error\n", top_app);
                goto reset; // 失败后直接跳过Stop部分
            }
            if (newPid == 0)
            {
                execlp(busybox_bin, "busybox", "chattr", "-R", "+i", top_app_dir, NULL);
                _exit(127);
            }
            else
            {
                int end = 0;
                if (waitpid(newPid, &end, 0) == -1)
                {
                    LOGPRINT(ANDROID_LOG_WARN, SERVER_NAME, "Stop %s: Wait Error\n", top_app);
                    goto reset; // 失败后直接跳过Stop部分
                }
                
                if (WIFEXITED(end) && WEXITSTATUS(end) == 0)
                {
                    LOGPRINT(ANDROID_LOG_INFO, SERVER_NAME, "Stop %s Success\n", top_app);
                }
                else
                {
                    LOGPRINT(ANDROID_LOG_WARN, SERVER_NAME, "Stop %s Failed\n", top_app);
                }
            }
        }
    }
    
    reset:
    
    if (strcmp(reset_app, "null") == 0)
    {
        return 0;
    }
    
    //检查缓存目录是否真实存在并过滤路径逃逸
    if (access(reset_app_dir, F_OK) == 0 &&
       strstr(reset_app_dir, "/../") == NULL)
    {
        pid_t newPid = fork();
        if (newPid == -1)
        {
            LOGPRINT(ANDROID_LOG_WARN, SERVER_NAME, "Reset %s: Fork Error\n", reset_app);
            return 1;
        }
        if (newPid == 0)
        {
            execlp(busybox_bin, "busybox", "chattr", "-R", "-i", reset_app_dir, NULL);
            _exit(127);
        }
        else
        {
            int end = 0;
            if (waitpid(newPid, &end, 0) == -1)
            {
                LOGPRINT(ANDROID_LOG_WARN, SERVER_NAME, "Reset %s: Wait Error\n", reset_app);
                return 1;
            }
            
            if (WIFEXITED(end) && WEXITSTATUS(end) == 0)
            {
                LOGPRINT(ANDROID_LOG_INFO, SERVER_NAME, "Reset %s Success\n", reset_app);
            }
            else
            {
                LOGPRINT(ANDROID_LOG_WARN, SERVER_NAME, "Reset %s Failed\n", reset_app);
            }
        }
    }
    return 0;
}
