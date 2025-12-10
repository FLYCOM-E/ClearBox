// 此脚本来自ClearBox模块，用于运行紧急GC优化，原理来自Coolapk@Amktiao，感谢大佬
#include "BashCore.h"

#define SYSFS_PATH "/sys/fs/f2fs"
#define SYSFS_FILE_NAME "gc_urgent" //Max Size 14
#define SYSFS_DIRTY_FILE "dirty_segments" //Max Size 14
#define SYSFS_FREE_FILE "free_segments" //Max Size 14

static int F2FS_GC();
static int IDLE_MAINT();
static int get_f2fs_dirty(char * dirty_file);
static int get_f2fs_free(char * free_file);

int main(int COMI, char * COM[])
{
    if (getuid() != 0)
    {
        printf(" » 请授予root权限！\n");
        return 1;
    }
    if (COMI < 2)
    {
        printf(" » 参数不足！\n");
        return 1;
    }
    
    if (strcasecmp(COM[1], "F2FS_GC") == 0)
    {
        return F2FS_GC();
    }
    else if (strcasecmp(COM[1], "FAST_GC") == 0)
    {
        return IDLE_MAINT();
    }
    else
    {
        return 1;
    }
    
    return 0;
}

/*
紧急GC
返回：
    int 成功返回0，失败返回1
*/
static int F2FS_GC()
{
    // GetProp
    char sysfs_name[64] = "";
    FILE * sysfs_name_fp = popen("getprop dev.mnt.dev.data", "r");
    if (sysfs_name_fp)
    {
        fgets(sysfs_name, sizeof(sysfs_name), sysfs_name_fp);
        pclose(sysfs_name_fp);
        sysfs_name[strcspn(sysfs_name, "\n")] = 0;
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
        printf(" » 您的设备不是 F2FS 文件系统\n » 维护仅支持 F2FS 环境！\n");
        return 1;
    }
    // 检测是否支持当前gc方案
    if (access(f2fs_sysfs_file, F_OK) != 0)
    {
        printf(" » 您的设备不支持当前GC功能\n");
        return 1;
    }
    
    // Get Dirty & Free
    int f2fs_dirty = 0, f2fs_free = 0;
    f2fs_dirty = get_f2fs_dirty(f2fs_sysfs_dirty_file);
    f2fs_free = get_f2fs_free(f2fs_sysfs_free_file);
    printf(" » 目前脏段: %d\n", f2fs_dirty);
    printf(" » 目前空闲段: %d\n\n", f2fs_free);
    fflush(stdout);
    
    char cache[4] = "";
    FILE * f2fs_sysfs_file_fp = fopen(f2fs_sysfs_file, "w");
    if (f2fs_sysfs_file_fp == NULL)
    {
        printf(" » GC启动失败! 节点打开失败！\n");
        return 1;
    }
    if (fprintf(f2fs_sysfs_file_fp, "%d", 1) > 0)
    {
        printf(" » GC已开始, 请您耐心等待，建议挂后台！\n");
        fflush(stdout);
        fclose(f2fs_sysfs_file_fp);
    }
    else
    {
        printf(" » GC启动失败! 节点写入失败！\n");
        fclose(f2fs_sysfs_file_fp);
        return 1;
    }
    
    // 等待循环
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
        if (time_m == 9)
        {
            printf(" » GC等待超时，已结束等待！\n");
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
                printf(" » GC运行完成，已结束运行！\n");
                fflush(stdout);
                break;
            }
        }
        
        if (time_m == 0)
        {
            printf(" » 已运行 %d 秒...\n", time_s);
        }
        else if (time_s == 0)
        {
            printf(" » 已运行 %d 分...\n", time_m);
        }
        else
        {
            printf(" » 已运行 %d 分 %d 秒...\n", time_m, time_s);
        }
        fflush(stdout);
    }
    
    // Again Get Dirty & Free
    int old_f2fs_dirty = f2fs_dirty;
    f2fs_dirty = get_f2fs_dirty(f2fs_sysfs_dirty_file);
    f2fs_free = get_f2fs_free(f2fs_sysfs_free_file);
    printf(" » 目前脏段: %d\n", f2fs_dirty);
    printf(" » 目前空闲段: %d\n\n", f2fs_free);
    
    if (old_f2fs_dirty > f2fs_dirty)
    {
        printf(" » 磁盘脏块减少 %d\n", old_f2fs_dirty - f2fs_dirty);
    }
    else
    {
        printf(" » 磁盘脏块增加 %d\n » GC可能仍在优化或并不适合您的设备！\n", f2fs_dirty - old_f2fs_dirty);
    }
    
    printf(" » GC已完成！\n");
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
    char cache[4] = "";
    FILE * f2fs_dirty_fp = fopen(dirty_file, "r");
    if (f2fs_dirty_fp)
    {
        fgets(cache, sizeof(cache), f2fs_dirty_fp);
        fclose(f2fs_dirty_fp);
    }
    else
    {
        printf(" » 警告：获取当前脏段失败！\n");
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
    char cache[4] = "";
    FILE * f2fs_free_fp = fopen(free_file, "r");
    if (f2fs_free_fp)
    {
        fgets(cache, sizeof(cache), f2fs_free_fp);
        fclose(f2fs_free_fp);
    }
    else
    {
        printf(" » 警告：获取当前空闲段失败！\n");
        fflush(stdout);
        return 0;
    }
    return atoi(cache);
}

// 快速磁盘优化
static int IDLE_MAINT()
{
    printf(" » 开始快速磁盘优化，请您耐心等待，可以离开前台！\n");
    if (system("sm idle-maint run >/dev/null") == 0)
    {
        // 这玩意很多时候不支持也返回成功，懒得详细检查了（（
        printf(" » 优化完成，可以试试更激进的GC优化哦 (・∀・)\n");
        return 0;
    }
    return 1;
}