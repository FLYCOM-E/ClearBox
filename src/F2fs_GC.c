/*
                    GNU GENERAL PUBLIC
                        Version 3

        此 Code 来自 ClearBox 模块，用于运行紧急GC优化
             原理来自 Coolapk@Amktiao，感谢大佬
*/

#include "INCLUDE/BashCore.h"

#define PROP "dev.mnt.dev.data"             // 路径名称属性
#define TIMEOUT 9                           // 超时时间（单位 M）
#define SYSFS_PATH "/sys/fs/f2fs"           // F2FS sysfs 路径
#define SYSFS_FILE_NAME "gc_urgent"         // 节点设备名
#define SYSFS_DIRTY_FILE "dirty_segments"   // 脏段节点名
#define SYSFS_FREE_FILE "free_segments"     // 自由段节点名

static int f2fs_gc(void);
static int fast_gc(void);
static long get_f2fs_dirty(char * dirty_file);
static long get_f2fs_free(char * free_file);

int disk_gc(int mode)
{
    if (mode == 0)
    {
        return f2fs_gc();
    }
    else if (mode == 1)
    {
        return fast_gc();
    }
    
    return 0;
}

/*
紧急GC
返回：
    int 成功返回 0，失败返回 1
*/
static int f2fs_gc(void)
{
    char sysfs_name[PROP_VALUE_MAX] = {0};
    if (getprop(PROP, sysfs_name) <= 0)
    {
        return 1;
    }
    
    char f2fs_sysfs_path[strlen(SYSFS_PATH) + strlen(sysfs_name) + 2];
    snprintf(f2fs_sysfs_path, sizeof(f2fs_sysfs_path), "%s/%s", SYSFS_PATH, sysfs_name);
    char f2fs_sysfs_file[strlen(f2fs_sysfs_path) + sizeof(SYSFS_FILE_NAME) + 2];
    snprintf(f2fs_sysfs_file, sizeof(f2fs_sysfs_file), "%s/%s", f2fs_sysfs_path, SYSFS_FILE_NAME);
    char f2fs_sysfs_dirty_file[strlen(f2fs_sysfs_path) + sizeof(SYSFS_DIRTY_FILE) + 2];
    snprintf(f2fs_sysfs_dirty_file, sizeof(f2fs_sysfs_dirty_file), "%s/%s", f2fs_sysfs_path, SYSFS_DIRTY_FILE);
    char f2fs_sysfs_free_file[strlen(f2fs_sysfs_path) + sizeof(SYSFS_FREE_FILE) + 2];
    snprintf(f2fs_sysfs_free_file, sizeof(f2fs_sysfs_free_file), "%s/%s", f2fs_sysfs_path, SYSFS_FREE_FILE);
    
    // 检测是否为 f2fs 文件系统
    if (access(f2fs_sysfs_path, F_OK) != 0)
    {
        fprintf(stderr, L_FG_ERR_NOF2FS);
        return 1;
    }
    // 检测是否支持当前 gc 方案
    if (access(f2fs_sysfs_file, F_OK) != 0)
    {
        fprintf(stderr, L_FG_ERR_CHECK);
        return 1;
    }
    
    // 获取脏段/自由段
    long f2fs_dirty = 0, f2fs_free = 0;
    f2fs_dirty = get_f2fs_dirty(f2fs_sysfs_dirty_file);
    f2fs_free = get_f2fs_free(f2fs_sysfs_free_file);
    fprintf(stderr, L_FG_DIRTY, f2fs_dirty);
    fprintf(stderr, L_FG_FREE, f2fs_free);
    
    // 打开节点并触发 GC
    FILE * f2fs_sysfs_file_fp = fopen(f2fs_sysfs_file, "w");
    if (f2fs_sysfs_file_fp == NULL)
    {
        fprintf(stderr, L_OPEN_FILE_FAILED, f2fs_sysfs_file, strerror(errno));
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
    
    // 等待循环
    char cache[16] = "";
    int time_s = 0, time_m = 0;
    
    for ( ; ; )
    {
        sleep(5);
        time_s += 5;
        
        if (time_s == 60)
        {
            time_s = 0;
            time_m++;
        }
        if (time_m == TIMEOUT)
        {
            fprintf(stderr, L_FG_ERR_TIMEOUT);
            break; // 超时退出
        }
        
        FILE * sysfs_file_fp = fopen(f2fs_sysfs_file, "r");
        if (sysfs_file_fp)
        {
            fgets(cache, sizeof(cache), sysfs_file_fp);
            fclose(sysfs_file_fp);
            if (strtol(cache, NULL, 10) == 0)
            {
                fprintf(stderr, L_FG_END);
                break; // GC 完成
            }
        }
        else
        {
            fprintf(stderr, L_OPEN_FILE_FAILED, f2fs_sysfs_file, strerror(errno));
        }
        
        // 这是一个时间打印逻辑
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
    
    // 读取最新脏段/空闲段
    long old_f2fs_dirty = f2fs_dirty;
    f2fs_dirty = get_f2fs_dirty(f2fs_sysfs_dirty_file);
    f2fs_free = get_f2fs_free(f2fs_sysfs_free_file);
    fprintf(stderr, L_FG_DIRTY, f2fs_dirty);
    fprintf(stderr, L_FG_FREE, f2fs_free);
    
    // 如果脏段反增说明可能此设备不支持或 GC 未完全完成
    if (old_f2fs_dirty > f2fs_dirty)
    {
        fprintf(stderr, L_FG_END_DIRTY, old_f2fs_dirty - f2fs_dirty);
    }
    else
    {
        fprintf(stderr, L_FG_END_DIRTY_2, f2fs_dirty - old_f2fs_dirty);
    }
    
    fprintf(stderr, L_FG_DONE);
    return 0;
}

/*
获取磁盘脏段
接收：
    char * dirty_file 脏段节点（完整路径）
返回：
    int 成功返回脏段数量，失败返回 0（与脏段为0同）
*/
static long get_f2fs_dirty(char * dirty_file)
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
        fprintf(stderr, L_OPEN_FILE_FAILED, dirty_file, strerror(errno));
        return 0;
    }
    return strtol(cache, NULL, 10);
}

/*
获取磁盘空闲段
接收：
    char * dirty_file 空闲段节点（完整路径）
返回：
    int 成功返回空闲段数量，失败返回 0（与空闲段为 0 同）
*/
static long get_f2fs_free(char * free_file)
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
        fprintf(stderr, L_OPEN_FILE_FAILED, free_file, strerror(errno));
        return 0;
    }
    return strtol(cache, NULL, 10);
}

// 快速磁盘优化
static int fast_gc(void)
{
    // 这是一个说慢不慢说快不快的功能，不检查成功，因为部分设备可能不支持
    system("sm idle-maint run >/dev/null 2>&1");
    system("sm fstrim >/dev/null 2>&1");
    printf(L_FG_FAST_GC_DONE);
    return 0;
}
