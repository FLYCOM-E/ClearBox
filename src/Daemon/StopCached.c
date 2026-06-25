/*
                    GNU GENERAL PUBLIC
                        Version 3

  此 Code 来自 ClearBox 模块，用于 App 自动缓存阻止功能（DAEMON）
*/

#include "../INCLUDE/main.h"

#define SERVER_NAME "StopCached"        // 进程名（MAX 15）
#define DATA_DIR "/data/data"           // 软件数据根目录
#define ROM_NAME "RunStart"             // 储存文件名
#define WHITELIST_NAME "whitelist.prop" // 白名单文件名
#define GET_TOPAPP "dumpsys activity lru | grep TOP | head -n 1 | cut -f3 -d ':' | cut -f1 -d '/'"
#define MAX_WHITELIST_APP 512               // 最大白名单数量

static void set_app_cache(char * top_app,
                        char * reset_app,
                        int skip_reset,
                        char whitelist[][NAME_MAX]);
static int read_whitelist(char * whitelist_file,
                          char whitelist[][NAME_MAX],
                          int * read_whitelist_app);

int stop_cache_daemon(char * argv[])
{
    // 定义储存文件
    char rom_file[strlen(work_dir) + sizeof(ROM_NAME) + 2],
         whitelist_file[strlen(work_dir) + strlen(WHITELIST_NAME) + 2];
    snprintf(rom_file, sizeof(rom_file), "%s/%s", work_dir, ROM_NAME);
    snprintf(whitelist_file, sizeof(whitelist_file), "%s/%s", work_dir, WHITELIST_NAME);
    
    // 定义待处理 app 临时储存变量
    char top_app_list[5][NAME_MAX] = {0},
         reset_app[NAME_MAX] = "",
         whitelist[MAX_WHITELIST_APP][NAME_MAX] = {0};
    
    // 提取 RunStart 储存值
    if (access(rom_file, F_OK) == 0)
    {
        char tmp[16] = "";
        char rom_key_line[NAME_MAX] = "";
        char * rom_key_line_p = NULL;
        FILE * rom_file_fp_r = fopen(rom_file, "r");
        if (rom_file_fp_r)
        {
            while (fgets(rom_key_line, sizeof(rom_key_line), rom_file_fp_r) != NULL)
            {
                rom_key_line[strcspn(rom_key_line, "\n")] = 0;
                // 如没找到对应值或值为空则跳过（保持原空值）
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
            write_log(work_dir, SERVER_NAME, L_OPEN_FILE_FAILED, rom_file, strerror(errno));
        }
    }
    
    // 读取白名单
    int read_whitelist_app = 0;
    read_whitelist(whitelist_file, whitelist, &read_whitelist_app);
    
    // INOTIFY
    int inotify_fd = inotify_init1(IN_NONBLOCK);
    if (inotify_fd == -1)
    {
        return -1;
    }
    int inotify_wd = inotify_add_watch(inotify_fd, rom_file, IN_DELETE_SELF | IN_CLOSE_WRITE);
    if (inotify_wd == -1)
    {
        return -1;
    }
    char inotify_buffer[PATH_MAX] = "";
    int watch = 1;
    
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
        return -1;
    }
    
    // 脱离终端
    if (s_daemon() != 0)
    {
        write_log(work_dir, SERVER_NAME, L_SERVER_START_ERR, strerror(errno));
        return -1;
    }
    if (s_signal() != 0)
    {
        write_log(work_dir, SERVER_NAME, L_SERVER_START_ERR, strerror(errno));
        return -1;
    }
    set_server_name(argv, SERVER_NAME);
    post(SERVER_NAME, SERVER_NAME, L_SCD_START_SUCCESS, getpid());
    write_log(work_dir, SERVER_NAME, L_SCD_START_SUCCESS, getpid());
    
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
    
    // Start
    while (sig_flag == 0)
    {
        // 检查获取前台失败次数
        if (get_error == max_get_error)
        {
            post(SERVER_NAME, SERVER_NAME, L_SCD_GETAPP_ERR_EXIT);
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
        
        if (watch == 1)
        {
            ssize_t len = read(inotify_fd, inotify_buffer, sizeof(inotify_buffer));
            if (len != -1)
            {
                char * inotify_buffer_p = inotify_buffer;
                char * inotify_len_p = inotify_buffer + len;
                while (inotify_buffer_p < inotify_len_p)
                {
                    struct inotify_event * event = (struct inotify_event *)inotify_buffer_p;
                    if (event -> mask & IN_DELETE_SELF)
                    {
                        inotify_rm_watch(inotify_fd, (uint32_t)inotify_wd);
                        watch = 0;
                        break;
                    }
                    if (event -> mask & IN_CLOSE_WRITE)
                    {
                        read_whitelist(whitelist_file, whitelist, &read_whitelist_app);
                    }
                    
                    inotify_buffer_p += sizeof(struct inotify_event) + event -> len;
                }
            }
        }
        else
        {
            if (access(rom_file, F_OK) == 0)
            {
                inotify_add_watch(inotify_fd, rom_file, IN_DELETE_SELF | IN_CLOSE_WRITE);
                watch = 1;
            }
        }
        
        // 获取前台软件包名
        char top_app[NAME_MAX] = "";
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
        
        // 更新当前包名记录
        snprintf(reset_app, sizeof(reset_app), "%s", top_app_list[4]);
        snprintf(top_app_list[4], sizeof(top_app_list[4]), "%s", top_app_list[3]);
        snprintf(top_app_list[3], sizeof(top_app_list[3]), "%s", top_app_list[2]);
        snprintf(top_app_list[2], sizeof(top_app_list[2]), "%s", top_app_list[1]);
        snprintf(top_app_list[1], sizeof(top_app_list[1]), "%s", top_app_list[0]);
        snprintf(top_app_list[0], sizeof(top_app_list[0]), "%s", top_app);
        
        FILE * rom_file_fp_w = fopen(rom_file, "w");
        if (rom_file_fp_w)
        {
            // 写入失败不检查
            fprintf(rom_file_fp_w,
                    "1=%s\n2=%s\n3=%s\n4=%s\n5=%s\nreset=%s",
                    top_app_list[0],
                    top_app_list[1],
                    top_app_list[2],
                    top_app_list[3],
                    top_app_list[4],
                    reset_app);
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
            set_app_cache(top_app_list[0], reset_app, skip_reset, whitelist);
        }
        // 循环返回 ===
    }
    close(inotify_fd);
    
    if (sig_flag == SIGHUP)
    {
        execv("/proc/self/exe", argv);
    }
    
    return 0;
}

/*
缓存阻止/恢复
接收：
    char * top_app 前台App包名
    char * reset_app 待恢复App包名
    int skip_reset 是否跳过恢复
    char whitelist[][NAME_MAX] 白名单列表
*/
static void set_app_cache(char * top_app,
                        char * reset_app,
                        int skip_reset,
                        char whitelist[][NAME_MAX])
{
    char top_app_dir[sizeof(DATA_DIR) + strlen(top_app) + 16],
         reset_app_dir[sizeof(DATA_DIR) + strlen(reset_app) + 16];
    snprintf(top_app_dir, sizeof(top_app_dir), "%s/%s/cache", DATA_DIR, top_app);              //topApp缓存目录定义
    snprintf(reset_app_dir, sizeof(reset_app_dir), "%s/%s/cache", DATA_DIR, reset_app);         //resetApp缓存目录定义
    
    // WHITELIST CHECK
    int in_whitelist = 0;
    for (int i = 0; i < MAX_WHITELIST_APP; i++)
    {
        if (strcmp(whitelist[i], "END") == 0)
        {
            break;
        }
        if (strcmp(whitelist[i], top_app) == 0)
        {
            in_whitelist = 1;
            break;
        }
    }
    
    //检查缓存目录是否真实存在并过滤路径逃逸
    if (in_whitelist != 1)
    {
        if (access(top_app_dir, F_OK) == 0 &&
            strstr(top_app_dir, "/../") == NULL)
        {
            if (s_chattr(top_app_dir, 1, 1) == 0)
            {
                print_log(ANDROID_LOG_INFO, SERVER_NAME, "Stop %s Success\n", top_app);
            }
            else
            {
                print_log(ANDROID_LOG_WARN, SERVER_NAME, "Stop %s Failed: %s\n", top_app, strerror(errno));
            }
        }
    }
    
    // 配合前面，skip_reset 为 1 不执行恢复
    if (skip_reset == 1)
    {
        return;
    }
    
    //检查缓存目录是否真实存在并过滤路径逃逸
    if (access(reset_app_dir, F_OK) == 0 &&
       strstr(reset_app_dir, "/../") == NULL)
    {
        if (s_chattr(reset_app_dir, 0, 1) == 0)
        {
            print_log(ANDROID_LOG_INFO, SERVER_NAME, "Reset %s Success\n", reset_app);
        }
        else
        {
            print_log(ANDROID_LOG_WARN, SERVER_NAME, "Reset %s Failed: %s\n", reset_app, strerror(errno));
        }
    }
    return;
}

/*
白名单读取/更新
接收：
    char * whitelist_file 白名单文件
    char whitelist[][NAME_MAX] 白名单列表数组
    int * read_whitelist_app 已读取白名单 App 数量
返回：
    成功返回 0，失败返回 -1
*/
static int read_whitelist(char * whitelist_file,
                          char whitelist[][NAME_MAX],
                          int * read_whitelist_app)
{
    int update = 0;
    if (* read_whitelist_app != 0)
    {
        update = 1;
    }
    
    FILE * fp = fopen(whitelist_file, "r");
    if (fp == NULL)
    {
        write_log(work_dir, SERVER_NAME, L_OPEN_FILE_FAILED, whitelist_file, strerror(errno));
        return -1;
    }
    else
    {
        int count = 0;
        char line[NAME_MAX + 1] = "";
        while (fgets(line, sizeof(line), fp))
        {
            line[strcspn(line, "\n")] = 0;
            if (strlen(line) <= 1)
            {
                continue;
            }
            
            // 如果为更新并且当前行 App 与上次记录不一致则解锁此 App
            if (update == 1 && strcmp(whitelist[count], line) != 0)
            {
                // TODO：这里有点开销，怎么优化呢...目前规模不大，如果以后拓展，这里最好改成哈希计算
                int is_new = 1;
                for (int i = 0; i < * read_whitelist_app; i++)
                {
                    if (strcmp(whitelist[i], line) == 0)
                    {
                        is_new = 0;
                    }
                }
                
                if (is_new)
                {
                    char app_dir[sizeof(DATA_DIR) + strlen(line) + 16];
                    snprintf(app_dir, sizeof(app_dir), "%s/%s/cache", DATA_DIR, line);
                    if (access(app_dir, F_OK) == 0)
                    {
                        if (s_chattr(app_dir, 0, 1) == 0)
                        {
                            print_log(ANDROID_LOG_INFO, SERVER_NAME, "Reset %s Success\n", line);
                        }
                        else
                        {
                            print_log(ANDROID_LOG_WARN, SERVER_NAME, "Reset %s Failed: %s\n", line, strerror(errno));
                        }
                    }
                }
            }
            
            snprintf(whitelist[count], sizeof(whitelist[count]), "%s", line);
            if ((count + 1) != MAX_WHITELIST_APP)
            {
                count++;
            }
            else
            {
                break;
            }
        }
        fclose(fp);
        
        * read_whitelist_app = count;
        if ((count + 1) != MAX_WHITELIST_APP)
        {
            snprintf(whitelist[count + 1], sizeof(whitelist[count + 1]), "END");
        }
    }
    
    return 0;
}
