// 此Code来自ClearBox模块，用于运行紧急GC优化，原理来自 Coolapk@Amktiao，感谢大佬
#include "INCLUDE/BashCore.h"

#define PROP "dev.mnt.dev.data" // 存储设备属性
#define TIMEOUT 9 // 超时时间，单位：分钟
#define SYSFS_PATH "/sys/fs/f2fs" // F2FS sysfs路径
#define SYSFS_FILE_NAME "gc_urgent" //Max Size 14
#define SYSFS_DIRTY_FILE "dirty_segments" //Max Size 14
#define SYSFS_FREE_FILE "free_segments" //Max Size 14

static int f2fs_gc(void);
static int fast_gc(void);
static int get_f2fs_dirty(char * dirty_file);
static int get_f2fs_free(char * free_file);

int main(int argc, char * argv[])
{
    if (getuid() != 0)
    {
        fprintf(stderr, L_NOT_USE_ROOT);
        return 1;
    }
    if (argc < 2)
    {
        fprintf(stderr, L_ARGS_FAILED);
        return 1;
    }
    
    if (strcasecmp(argv[1], "F2FS_GC") == 0)
    {
        return f2fs_gc();
    }
    else if (strcasecmp(argv[1], "FAST_GC") == 0)
    {
        return fast_gc();
    }
    else
    {
        fprintf(stderr, L_ARGS_FAILED_2);
        return 1;
    }
    
    return 0;
}

/*
紧急GC
返回：
    int 成功返回0，失败返回1
*/
static int f2fs_gc(void)
{
    // GetProp
    char sysfs_name[PROP_VALUE_MAX] = {0};
    if (getprop(PROP, sysfs_name) <= 0)
    {
        return 1;
    }
    
    // F2FS_SYSFS_PATH
    char f2fs_sysfs_path[strlen(SYSFS_PATH) + strlen(sysfs_name) + 2];
    snprintf(f2fs_sysfs_path, sizeof(f2fs_sysfs_path), "%s/%s", SYSFS_PATH, sysfs_name);
    // F2FS_SYSFS_FILE
    char f2fs_sysfs_file[strlen(f2fs_sysfs_path) + 16];
    snprintf(f2fs_sysfs_file, sizeof(f2fs_sysfs_file), "%s/%s", f2fs_sysfs_path, SYSFS_FILE_NAME);
    // F2FS_SYSFS_DIRTY_FILE
    char f2fs_sysfs_dirty_file[strlen(f2fs_sysfs_path) + 16];
    snprintf(f2fs_sysfs_dirty_file, sizeof(f2fs_sysfs_dirty_file), "%s/%s", f2fs_sysfs_path, SYSFS_DIRTY_FILE);
    // F2FS_SYSFS_FREE_FILE
    char f2fs_sysfs_free_file[strlen(f2fs_sysfs_path) + 16];
    snprintf(f2fs_sysfs_free_file, sizeof(f2fs_sysfs_free_file), "%s/%s", f2fs_sysfs_path, SYSFS_FREE_FILE);
    
    // 检测是否为f2fs文件系统
    if (access(f2fs_sysfs_path, F_OK) != 0)
    {
        fprintf(stderr, L_FG_ERR_NOF2FS);
        return 1;
    }
    // 检测是否支持当前gc方案
    if (access(f2fs_sysfs_file, F_OK) != 0)
    {
        fprintf(stderr, L_FG_ERR_CHECK);
        return 1;
    }
    
    // Get Dirty & Free
    int f2fs_dirty = 0, f2fs_free = 0;
    f2fs_dirty = get_f2fs_dirty(f2fs_sysfs_dirty_file);
    f2fs_free = get_f2fs_free(f2fs_sysfs_free_file);
    fprintf(stderr, L_FG_DIRTY, f2fs_dirty);
    fprintf(stderr, L_FG_FREE, f2fs_free);
    fflush(stdout);
    
    FILE * f2fs_sysfs_file_fp = fopen(f2fs_sysfs_file, "w");
    if (f2fs_sysfs_file_fp == NULL)
    {
        fprintf(stderr, L_FG_ERR_OPENSYSFS);
        return 1;
    }
    if (fprintf(f2fs_sysfs_file_fp, "%d", 1) > 0)
    {
        fprintf(stderr, L_FG_START);
        fclose(f2fs_sysfs_file_fp);
    }
    else
    {
        fprintf(stderr, L_FG_ERR_WRITESYSFS);
        fclose(f2fs_sysfs_file_fp);
        return 1;
    }
    fflush(stdout);
    
    // 等待循环
    char cache[16] = "";
    // 秒 / 分
    int time_s = 0, time_m = 0;
    for ( ; ; )
    {
        time_s += 5;
        sleep(5);
        
        if (time_s == 60)
        {
            time_s = 0;
            time_m++;
        }
        if (time_m == TIMEOUT)
        {
            fprintf(stderr, L_FG_ERR_TIMEOUT);
            fflush(stdout);
            break;
        }
        
        FILE * sysfs_file_fp = fopen(f2fs_sysfs_file, "r");
        if (sysfs_file_fp)
        {
            fgets(cache, sizeof(cache), sysfs_file_fp);
            fclose(sysfs_file_fp);
            if (atoi(cache) == 0)
            {
                fprintf(stderr, L_FG_END);
                fflush(stdout);
                break;
            }
        }
        
        if (time_m == 0)
        {
            printf(L_FG_RUN_S, time_s);
        }
        else if (time_s == 0)
        {
            printf(L_FG_RUN_M, time_m);
        }
        else
        {
            printf(L_FG_RUN_MS, time_m, time_s);
        }
        fflush(stdout);
    }
    
    // Again Get Dirty & Free
    int old_f2fs_dirty = f2fs_dirty;
    f2fs_dirty = get_f2fs_dirty(f2fs_sysfs_dirty_file);
    f2fs_free = get_f2fs_free(f2fs_sysfs_free_file);
    fprintf(stderr, L_FG_DIRTY, f2fs_dirty);
    fprintf(stderr, L_FG_FREE, f2fs_free);
    
    if (old_f2fs_dirty > f2fs_dirty)
    {
        fprintf(stderr, L_FG_END_DIRTY, old_f2fs_dirty - f2fs_dirty);
    }
    else
    {
        fprintf(stderr, L_FG_END_DIRTY_2, f2fs_dirty - old_f2fs_dirty);
    }
    
    fprintf(stderr, L_FG_DONE);
    fflush(stdout);
    return 0;
}

/*
获取磁盘脏段
接收：
    char * dirty_file 脏段节点（完整路径）
返回：
    int 成功返回脏段数量，失败返回0（与脏段为0同）
*/
static int get_f2fs_dirty(char * dirty_file)
{
    char cache[16] = "";
    FILE * f2fs_dirty_fp = fopen(dirty_file, "r");
    if (f2fs_dirty_fp)
    {
        fgets(cache, sizeof(cache), f2fs_dirty_fp);
        fclose(f2fs_dirty_fp);
    }
    else
    {
        printf(L_FG_W_GET_DIRTY);
        fflush(stdout);
        return 0;
    }
    return atoi(cache);
}

/*
获取磁盘空闲段
接收：
    char * dirty_file 空闲段节点（完整路径）
返回：
    int 成功返回空闲段数量，失败返回0（与空闲段为0同）
*/
static int get_f2fs_free(char * free_file)
{
    char cache[16] = "";
    FILE * f2fs_free_fp = fopen(free_file, "r");
    if (f2fs_free_fp)
    {
        fgets(cache, sizeof(cache), f2fs_free_fp);
        fclose(f2fs_free_fp);
    }
    else
    {
        printf(L_FG_W_GET_FREE);
        fflush(stdout);
        return 0;
    }
    return atoi(cache);
}

// 快速磁盘优化
static int fast_gc(void)
{
    // 这是一个说慢不慢说快不快的功能
    system("sm idle-maint run >/dev/null 2>&1");
    system("sm fstrim >/dev/null 2>&1");
    // 不检查成功与否
    printf(L_FG_FAST_GC_DONE);
    return 0;
}
