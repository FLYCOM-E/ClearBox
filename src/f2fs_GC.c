// 此脚本来自ClearBox模块，用于运行紧急GC优化，原理来自Coolapk@Amktiao，感谢大佬
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define SYSFS_PATH "/sys/fs/f2fs"
#define SYSFS_FILE_NAME "gc_urgent"
#define SYSFS_DIRTY_FILE "dirty_segments"
#define SYSFS_FREE_FILE "free_segments"

static int F2FS_GC();
static int IDLE_MAINT();

int main(int COMI, char * COM[])
{
    if (getuid() != 0)
    {
        printf(" » 请授予root权限！\n");
        return 1;
    }
    if (COMI < 2)
    {
        printf(" » ERROR：需要一个参数，未传入选项名称！\n");
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
    int dirty = 0, free = 0;
    
    char dirty_cache[4] = "";
    FILE * dirty_fp = fopen(f2fs_sysfs_dirty_file, "r");
    if (dirty_fp)
    {
        fgets(dirty_cache, sizeof(dirty_cache), dirty_fp);
        dirty = atoi(dirty_cache);
    }
    else
    {
        printf(" » 警告：获取当前脏段失败！\n");
    }
    char free_cache[4] = "";
    FILE * free_fp = fopen(f2fs_sysfs_free_file, "r");
    if (free_fp)
    {
        fgets(free_cache, sizeof(free_cache), free_fp);
        free = atoi(free_cache);
    }
    else
    {
        printf(" » 警告：获取当前空闲段失败！\n");
    }
    
    printf(" » 目前脏段: %d\n", dirty);
    printf(" » 目前空闲段: %d\n\n", free);
    
    char cache[4] = "";
    FILE * f2fs_sysfs_file_fp = fopen(f2fs_sysfs_file, "w");
    if (f2fs_sysfs_file_fp)
    {
        if (fprintf(f2fs_sysfs_file_fp, "%d", 1) > 0)
        {
            printf(" » GC已开始, 请您耐心等待，建议挂后台！\n");
        }
        else
        {
            printf(" » GC启动失败! 节点写入失败！\n");
            fclose(free_fp);
            fclose(dirty_fp);
            fclose(f2fs_sysfs_file_fp);
            return 1;
        }
        
        int time_s = 0, time_m = 0;
        for ( ; ; )
        {
            if (time_s == 60)
            {
                time_s = 0;
                time_m++;
            }
            if (time_m == 9)
            {
                printf(" » GC等待超时，已结束等待！\n");
                break;
            }
            fgets(cache, sizeof(cache), f2fs_sysfs_file_fp);
            if (atoi(cache) == 0)
            {
                printf(" » GC运行完成，已结束运行！\n");
                break;
            }
            
            printf(" » 已运行 %d 分 %d 秒...\n", time_m, time_s);
            time_s += 5;
            sleep(5);
        }
        fclose(f2fs_sysfs_file_fp);
    }
    
    int old_dirty = dirty;
    dirty = 0;
    free = 0;
    
    if (dirty_fp)
    {
        fgets(dirty_cache, sizeof(dirty_cache), dirty_fp);
        fclose(dirty_fp);
        dirty = atoi(dirty_cache);
    }
    else
    {
        printf(" » 警告：获取当前脏段失败！\n");
    }
    if (free_fp)
    {
        fgets(free_cache, sizeof(free_cache), free_fp);
        free = atoi(free_cache);
    }
    else
    {
        printf(" » 警告：获取当前空闲段失败！\n");
    }
    
    if (old_dirty > dirty)
    {
        printf(" » 磁盘脏块减少 %d\n", old_dirty - dirty);
    }
    else
    {
        printf(" » 磁盘脏块增加 %d\n » GC可能仍在优化或并不适合您的设备！\n", dirty - old_dirty);
    }
    
    fclose(free_fp);
    fclose(dirty_fp);
    printf(" » GC已完成！\n");
    return 0;
}

static int IDLE_MAINT()
{
    printf(" » 开始快速磁盘优化，请您耐心等待，可以离开前台！\n");
    if (system("sm idle-maint run >/dev/null") == 0)
    {
        printf(" » 优化完成，可以试试更激进的GC优化哦 (・∀・)\n");
        return 0;
    }
    return 1;
}