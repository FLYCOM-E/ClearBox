// By ClearBox StopCache
#include "../INCLUDE/BashCore.h"

#define DATA_DIR "/data/data" // 软件数据根目录
#define ROM_NAME "RunStart" //Max Size 30
#define WHITELIST_NAME "whitelist.prop" // 白名单文件
#define GET_TOPAPP "dumpsys activity lru | grep TOP | head -n 1 | cut -f3 -d ':' | cut -f1 -d '/'"
#define MICRO_CARD_PATH "/mnt/expand"
#define MAX_CARD 5 // 最大拓展卡数量
#define MAX_CARD_ID_LEN 256
#define LOGPRINT __android_log_print
#define SERVER_NAME "ClearBox StopCached"

static int set_app_cache(char * dir, char * top_app,
                        char * reset_app, char * work_dir,
                        char * bin_dir, int skip_reset);

int stop_cache_daemon(char * argv[], char * work_dir, char * bin_dir)
{
    // 检查及读取外部拓展储存
    int card_count = 0;
    char card_list[MAX_CARD][strlen(MICRO_CARD_PATH) + MAX_CARD_ID_LEN] = {0};
    
    if (access(MICRO_CARD_PATH, F_OK) == 0)
    {
        struct dirent * entry;
        DIR * micro_card_dp = opendir(MICRO_CARD_PATH);
        if (micro_card_dp)
        {
            while ((entry = readdir(micro_card_dp)))
            {
                if (strcmp(entry -> d_name, ".") == 0 ||
                    strcmp(entry -> d_name, "..") == 0)
                {
                    continue;
                }
                
                if (card_count < MAX_CARD)
                {
                    snprintf(card_list[card_count], sizeof(card_list[card_count]),
                             "%s/%s/user/0", MICRO_CARD_PATH, entry -> d_name);
                    if (access(card_list[card_count], F_OK) == 0)
                    {
                        card_count++;
                    }
                }
                else
                {
                    break;
                }
            }
            closedir(micro_card_dp);
        }
    }
    else
    {
        fprintf(stderr, L_OPEN_PATH_FAILED, MICRO_CARD_PATH, strerror(errno));
    }
    
    //定义储存文件
    char rom_file[strlen(work_dir) + 32];
    snprintf(rom_file, sizeof(rom_file), "%s/%s", work_dir, ROM_NAME);
    
    //定义待处理app临时储存变量
    char top_app_list[5][MAX_PACKAGE] = {0}, reset_app[MAX_PACKAGE] = "";
    
    //提取RunStart储存值（如有）
    if (access(rom_file, F_OK) == 0)
    {
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
                        rom_key_line_p = strchr(rom_key_line, '=');
                        if (rom_key_line_p)
                        {
                            snprintf(top_app_list[i], sizeof(top_app_list[i]), "%s", rom_key_line_p + 1);
                        }
                    }
                    i++;
                }
                if (strstr(rom_key_line, "reset="))
                {
                    rom_key_line_p = strchr(rom_key_line, '=');
                    if (rom_key_line_p)
                    {
                        snprintf(reset_app, sizeof(reset_app), "%s", rom_key_line_p + 1);
                    }
                }
            }
            fclose(rom_file_fp_r);
        }
        else
        {
            fprintf(stderr, L_OPEN_FILE_FAILED, rom_file, strerror(errno));
        }
    }
    
    // 如果包名含“ / ”则丢弃
    for (int i = 0; i < 5; i++)
    {
        if (strstr(top_app_list[i], "/") != NULL)
        {
            top_app_list[i][0] = '\0';
        }
    }
    
    // 设置命名空间
    if (set_name_space() != 0)
    {
        return 1;
    }
    
    // 创建子进程脱离终端
    pid_t new_pid = fork();
    if (new_pid == -1)
    {
        char post_text[sizeof(L_SERVER_START_ERR) + 128] = "";
        snprintf(post_text, sizeof(post_text), L_SERVER_START_ERR, strerror(errno));
        write_log(work_dir, SERVER_NAME, post_text);
        return 1;
    }
    else if (new_pid != 0)
    {
        exit(0);
    }
    int std = open("/dev/null", O_RDWR);
    dup2(std, STDIN_FILENO);
    dup2(std, STDOUT_FILENO);
    dup2(std, STDERR_FILENO);
    close(std);
    setsid();
    chdir("/");
    set_server_name(argv, SERVER_NAME);
    
    post(SERVER_NAME, L_SCD_START_SUCCESS);
    
    /* 
    等待时间
    深度休眠时间
    获取前台App失败次数
    最大获取前台App失败次数
    空循环计数
    深度休眠循环次数阀值
    */
    unsigned int cycle_time = 10,
        deep_time = 60,
        get_error = 0,
        max_get_error = 10,
        empty_count = 0,
        max_empty_count = 30;
    
    //Start the cycle
    for ( ; ; )
    {
        //检查获取前台失败次数
        if (get_error == max_get_error)
        {
            post(SERVER_NAME, L_SCD_GETAPP_ERR_EXIT);
            break;
        }
        if (empty_count >= max_empty_count)
        {
            // 减少唤醒
            // App 不变则保持 deep_time
            sleep(deep_time);
        }
        else
        {
            sleep(cycle_time);
        }
        
        // 获取前台软件包名
        char top_app[MAX_PACKAGE] = "";
        FILE * top_app_fp = popen(GET_TOPAPP, "r");
        if (top_app_fp == NULL)
        {
            get_error++;
            sleep(10); // 加上总循环共 >=20 秒额外等待
            continue;
        }
        // 读取包名
        if (fgets(top_app, sizeof(top_app), top_app_fp) == NULL)
        {
            // 为 NULL 一般是屏幕关闭
            empty_count++;
            pclose(top_app_fp);
            continue;
        }
        else
        {
            get_error = 0;
            pclose(top_app_fp);
            
            top_app[strcspn(top_app, "\n")] = 0;
            if (strcmp(top_app, top_app_list[0]) == 0)
            {
                empty_count++;
                continue;
            }
            else
            {
                empty_count = 0;
            }
        }
        
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
            // 这里不检查错误情况
            fprintf(rom_file_fp_w,
                    "1=%s\n2=%s\n3=%s\n4=%s\n5=%s\nreset=%s",
                    top_app_list[0],
                    top_app_list[1],
                    top_app_list[2],
                    top_app_list[3],
                    top_app_list[4],
                    reset_app
                   );
            fclose(rom_file_fp_w);
        }
        
        // 这个逻辑目标为跳过不必要的缓存加解锁
        // 如果 TopApp 已被阻止缓存将不再调用阻止缓存
        // 如果 reset_app 仍在缓存阻止列表则暂时不再恢复
        int skip_stop = 0, skip_reset = 0;
        for (int i = 0; i < 5; i++)
        {
            if (strcmp(top_app_list[0], top_app_list[i]) == 0)
            {
                if (i == 0)
                {
                    continue;
                }
                else
                {
                    skip_stop = 1;
                }
            }
            if (strcmp(reset_app, top_app_list[i]) == 0)
            {
                skip_reset = 1;
            }
        }
        
        // 调用处理函数，这里配合前面检查，skip_stop 为 1 则跳过
        if (skip_stop == 0)
        {
            // 内部储存
            set_app_cache(DATA_DIR, top_app_list[0], reset_app, work_dir, bin_dir, skip_reset);
            // 外部储存
            if (card_count > 0)
            {
                for (int i = 0; i < card_count; i++)
                {
                    set_app_cache(card_list[i], top_app_list[0], reset_app, work_dir, bin_dir, skip_reset);
                }
            }
        }
        // 循环返回 ===
    }
    return 1;
}

/*
缓存阻止实现
接收：
    char * dir 软件数据根目录
    char * top_app 前台App包名
    char * reset_app 待恢复App包名
    char * work_dir 配置目录
    char * bin_dir Bin目录
    int skip_reset 是否跳过恢复
返回：
    int 成功返回0，失败返回-1
    
*/
static int set_app_cache(char * dir, char * top_app,
                        char * reset_app, char * work_dir,
                        char * bin_dir, int skip_reset
                       )
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
        if (whitelist_check(whitelist_file, top_app) == 1)
        {
            in_whitelist = 1;
        }
        
        // in_whitelist = 0 时执行缓存阻止
        if (in_whitelist == 0)
        {
            pid_t newPid = fork();
            if (newPid == -1)
            {
                LOGPRINT(ANDROID_LOG_WARN, SERVER_NAME, "Stop %s: Fork Error\n", top_app);
                goto reset; // 失败直接跳过 Stop
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
    
    // 配合前面，skip_reset 为 1 不执行恢复
    if (skip_reset == 1)
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
