// 此脚本来自ClearBox模块，用于删除垃圾缓存及空文件夹
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAX_PACKAHE 256
#define SETTINGS_FILE_NAME "settings.prop"
#define WHITELIST "%s/ClearWhitelist.prop"
#define STORAGES_DIR "/storage/%s"
#define COMMAND_D "rm -r %s/* 2>/dev/null"
#define GET_SDCARD_ID "ls /storage | grep .*- 2>/dev/null"
#define DELETE_LOGFILE "\"%s/busybox\" find %s -type f -name \"*.log\" -delete"
#define DELETE_DIR "\"%s/busybox\" find %s -type d -empty -delete"

static int DeleteAppCache(char * data_path, char * work_dir);
static int CheckWhiteList(char * package, char * whitelist_file);
static int DelateMediaCache(char * bin_dir, char * storage_dir);

int main(int COMI, char * COM[])
{
    if (getuid() != 0)
    {
        printf(" » 请授予root权限！\n");
        return 1;
    }
    if (COMI < 5)
    {
        printf(" » 参数不够！\n");
        return 1;
    }
    
    // 循环获取传入配置文件、Bin文件目录
    char work_dir[64] = "", bin_dir[128] = "";
    for (int i = 0; i < COMI - 1; i++)
    {
        if (strcmp(COM[i], "-w") == 0)
        {
            if (strlen(COM[i + 1]) > 60)
            {
                printf(" » 配置路径过长！\n");
                return 1;
            }
            if (access(COM[i + 1], F_OK) != 0)
            {
                printf(" » 配置路径不存在/无法访问！\n");
                return 1;
            }
            snprintf(work_dir, sizeof(work_dir), "%s", COM[i + 1]);
        }
        if (strcmp(COM[i], "-b") == 0)
        {
            if (strlen(COM[i + 1]) > 60)
            {
                printf(" » Bin路径过长！\n");
                return 1;
            }
            if (access(COM[i + 1], F_OK) != 0)
            {
                printf(" » Bin路径不存在/无法访问！\n");
                return 1;
            }
            snprintf(bin_dir, sizeof(bin_dir), "%s", COM[i + 1]);
        }
    }
    if (strcmp(work_dir, "") == 0)
    {
        printf(" » 未传入配置路径！\n");
        return 1;
    }
    if (strcmp(bin_dir, "") == 0)
    {
        printf(" » 未传入Bin路径！\n");
        return 1;
    }
    
    int cleardisk = 0; // 是否清理外部储存
    char sdcard_id[16] = "", // 外置SD名称
         sdcard_dir[128] = "", // 外置SD完整路径
         data_dir[128] = "", // 内部储存完整路径
         settings_file[strlen(work_dir) + 16], // 设置信息储存文件
         settings_file_line[64] = "";
    
    // 获取外部储存名称（多储存暂不支持，后续...吧
    FILE * sdcard_id_fp = popen(GET_SDCARD_ID, "r");
    if (sdcard_id_fp)
    {
        fgets(sdcard_id, sizeof(sdcard_id), sdcard_id_fp);
        pclose(sdcard_id_fp);
        sdcard_id[strcspn(sdcard_id, "\n")] = 0;
        if (strcmp(sdcard_id, "") == 0)
        {
            strcpy(sdcard_id, "(null)");
        }
    }
    
    // 拼接路径
    snprintf(settings_file, sizeof(settings_file), "%s/%s", work_dir, SETTINGS_FILE_NAME);
    snprintf(data_dir, sizeof(data_dir), STORAGES_DIR, "emulated/0");
    snprintf(sdcard_dir, sizeof(sdcard_dir), STORAGES_DIR, sdcard_id);
    
    // 打开设置信息文件并查找对应值
    FILE * settings_file_fp = fopen(settings_file, "r");
    if (settings_file_fp)
    {
        while (fgets(settings_file_line, sizeof(settings_file_line), settings_file_fp))
        {
            settings_file_line[strcspn(settings_file_line, "\n")] = 0;
            if (strcmp(settings_file_line, "cleardisk=1") == 0)
            {
                cleardisk = 1;
            }
        }
        fclose(settings_file_fp);
    }
    
    // 处理内部储存
    if (DeleteAppCache(data_dir, work_dir) == 0 && DelateMediaCache(bin_dir, data_dir) == 0)
    {
        printf(" » 内部储存垃圾删除完成！\n");
    }
    if (cleardisk == 1) // 允许清理外部储存
    {
        if (access(sdcard_dir, F_OK) == 0) // 外部储存存在则处理
        {
            if (DeleteAppCache(sdcard_dir, work_dir) == 0 && DelateMediaCache(bin_dir, sdcard_dir) == 0)
            {
                printf(" » 外部储存 %s 垃圾删除完成！\n", sdcard_id);
            }
        }
    }
    
}

/* 
内/外部储存App缓存清理函数

传入：储存根目录、配置文件目录
返回：传入储存根目录不存在/无法打开返回 1，否则返回0

不检查传入配置目录，由下游白名单函数检查
*/
static int DeleteAppCache(char * data_path, char * work_dir)
{
    if (access(data_path, F_OK) != 0)
    {
        printf(" » 路径 %s 不存在/无权查看！\n", data_path);
        return 1;
    }
    
    // 拼接/定义路径
    int clean_count = 0;
    char whitelist_file[strlen(work_dir) + strlen(WHITELIST) + 2], app_data_path[strlen(data_path) + 32];
    snprintf(app_data_path, sizeof(app_data_path), "%s/Android/data", data_path);
    snprintf(whitelist_file, sizeof(whitelist_file), WHITELIST, work_dir);
    
    // 打开目录
    struct dirent * entry;
    DIR * app_data_dir_dp = opendir(app_data_path);
    if (app_data_dir_dp == NULL)
    {
        printf(" » 路径 %s 打开失败！！\n", data_path);
        return 1;
    }
    
    // 遍历目录
    while ((entry = readdir(app_data_dir_dp)))
    {
        if (strcmp(entry -> d_name, ".") == 0 ||
           strcmp(entry -> d_name, "..") == 0)
        {
            continue;
        }
        
        // 完整缓存路径
        char app_path[strlen(entry -> d_name) + strlen(app_data_path) + 32];
        snprintf(app_path, sizeof(app_path), "%s/%s/cache", app_data_path, entry -> d_name);
        if (access(app_path, F_OK) != 0)
        {
            continue;
        }
        // 白名单检查
        if (CheckWhiteList(entry -> d_name, whitelist_file) == 1)
        {
            continue;
        }
        
        // 清理命令
        char command[strlen(app_path) + 32];
        snprintf(command, sizeof(command), COMMAND_D, app_path);
        if (system(command) == 0)
        {
            printf(" » 删除 %s\n", entry -> d_name);
            clean_count++;
        }
        else
        {
            printf(" » 删除 %s 失败！\n", entry -> d_name);
        }
    }
    closedir(app_data_dir_dp);
    
    printf(" » 共清理 %d 个软件缓存！\n", clean_count);
    
    return 0;
}

/*
白名单检查函数

接收：软件包名、白名单文件
返回：白名单文件不存在返回 -1，找到返回 1，未找到返回 0
*/
static int CheckWhiteList(char * package, char * whitelist_file)
{
    if (access(whitelist_file, F_OK) != 0)
    {
        return -1;
    }
    char line[MAX_PACKAHE] = "";
    FILE * whilelist_file_fp = fopen(whitelist_file, "r");
    if (whilelist_file_fp)
    {
        while (fgets(line, sizeof(line), whilelist_file_fp))
        {
            line[strcspn(line, "\n")] = 0;
            if (strcmp(line, package) == 0)
            {
                fclose(whilelist_file_fp);
                return 1;
            }
        }
        fclose(whilelist_file_fp);
    }
    return 0;
}

/*
内部储存清理函数

接收：Bin文件路径、储存根目录
返回：任意传入路径不存在/打开失败返回 1，否则返回 0
*/
static int DelateMediaCache(char * bin_dir, char * storage_dir)
{
    if (access(bin_dir, F_OK) != 0)
    {
        return 1;
    }
    if (access(storage_dir, F_OK) != 0)
    {
        return 1;
    }
    
    // 打开目录
    struct dirent * entry;
    DIR * dir_dp = opendir(storage_dir);
    if (dir_dp == NULL)
    {
        return 1;
    }
    
    // 遍历目录
    while ((entry = readdir(dir_dp)))
    {
        if (strcmp(entry -> d_name, ".") == 0 ||
           strcmp(entry -> d_name, "..") == 0)
        {
            continue;
        }
        
        // 不想硬编码DCIM、Music......了，直接尝试查找删除所有路径下的吧
        char path[strlen(storage_dir) + strlen(entry -> d_name) + 32];
        snprintf(path, sizeof(path), "%s/%s/.thumbnails", storage_dir, entry -> d_name);
        
        if (access(path, F_OK) == 0)
        {
            char command[strlen(path) + 32];
            snprintf(command, sizeof(command), COMMAND_D, path);
            if (system(command) != 0)
            {
                printf(" » %s 删除失败！\n", path);
            }
        }
    }
    closedir(dir_dp);
    
    char d_log_command[strlen(storage_dir) + 64], d_dir_command[strlen(storage_dir) + 64];
    snprintf(d_log_command, sizeof(d_log_command), DELETE_LOGFILE, bin_dir, storage_dir);
    snprintf(d_dir_command, sizeof(d_dir_command), DELETE_DIR, bin_dir, storage_dir);
    
    if (system(d_log_command) == 0 && system(d_dir_command) == 0)
    {
        printf(" » 空文件夹、log文件清理成功！\n");
    }
    else
    {
        printf(" » 空文件夹、log文件清理失败！\n");
        /* 如果有人看到这里可能好奇为什么不返回 1
        因为觉得这个是否清理成功没必要影响函数返回值 */
    }
    
    return 0;
}
