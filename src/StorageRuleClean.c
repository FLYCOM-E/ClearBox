// 此Code来自ClearBox模块，用于自定义规则清理/干掉文件目录
#include "INCLUDE/BashCore.h"

#define CONFIG_DIR_NAME "清理配置" // 配置目录名称

int main(int argc, char * argv[])
{
    if (getuid() != 0)
    {
        fprintf(stderr, L_NOT_USE_ROOT);
        return 1;
    }
    
    argc--;
    argv++;
    if (argc < 1)
    {
        fprintf(stderr, L_ARGS_FAILED);
        return 1;
    }
    
    // 参数解析
    char * work_dir = NULL;
    while (argc > 1)
    {
        if (strcmp(argv[0], "-w") == 0)
        {
            if (strlen(argv[1]) > MAX_WORK_DIR_LEN)
            {
                fprintf(stderr, L_CONFIG_PATH_TOOLONG);
                return 1;
            }
            if (access(argv[1], F_OK) != 0)
            {
                fprintf(stderr, L_CONFIG_PATH_NOTFOUND);
                return 1;
            }
            work_dir = argv[1];
            argc -= 2;
            argv += 2;
        }
        else
        {
            fprintf(stderr, L_ARGS_FAILED_2);
            return 1;
        }
    }
    if (work_dir == NULL)
    {
        fprintf(stderr, L_ARG_CONFIGPATH_ERR);
        return 1;
    }
    
    /// 配置目录
    char config_dir[strlen(work_dir) + strlen(CONFIG_DIR_NAME) + 2];
    snprintf(config_dir, sizeof(config_dir), "%s/%s", work_dir, CONFIG_DIR_NAME);
    
    // 遍历配置目录
    struct dirent * config_file_name;
    DIR * config_dir_dp = opendir(config_dir);
    if (config_dir_dp == NULL)
    {
        fprintf(stderr, L_OPEN_PATH_FAILED, config_dir);
        return 1;
    }
    
    while ((config_file_name = readdir(config_dir_dp)))
    {
        if (strcmp(config_file_name -> d_name, ".") == 0 ||
           strcmp(config_file_name -> d_name, "..") == 0)
        {
            continue;
        }
        
        // 配置文件
        char config_file[strlen(config_file_name -> d_name) + strlen(config_dir) + 2];
        snprintf(config_file, sizeof(config_file), "%s/%s", config_dir, config_file_name -> d_name);
        
        FILE * config_file_fp = fopen(config_file, "r");
        if (config_file_fp == NULL)
        {
            fprintf(stderr, L_SR_OPEN_CONFIG_ERR, config_file_name -> d_name);
            continue;
        }
        
        printf(L_SR_START, config_file_name -> d_name);
        fflush(stdout);
        
        int count = 0;
        char config_file_line[256] = "", dir[256] = "";
        while (fgets(config_file_line, sizeof(config_file_line), config_file_fp))
        {
            config_file_line[strcspn(config_file_line, "\n")] = 0;
            
            count++;
            char * config_file_line_ptr = config_file_line;
            while (isspace(* config_file_line_ptr)) config_file_line_ptr++;
            
            // 如果该行被注释则返回
            if (* config_file_line_ptr == '#')
            {
                continue;
            }
            
            if (* config_file_line_ptr == '@')
            {
                //初始目录
                char * key_str = strchr(config_file_line, '@');
                if (key_str)
                {
                    snprintf(dir, sizeof(dir), "%s", key_str + 1);
                    if (access(dir, F_OK) != 0)
                    {
                        fprintf(stderr, L_SR_W_CONFIG_STATTPATH_ERR, config_file_name -> d_name);
                    }
                }
                continue;
            }
            
            // 判断line是否为绝对路径，否则根据初始目录拼接
            char path[strlen(config_file_line) + strlen(dir) + 16];
            if (* config_file_line_ptr == '/')
            {
                snprintf(path, sizeof(path), "%s", config_file_line);
            }
            else
            {
                snprintf(path, sizeof(path), "%s/%s", dir, config_file_line);
            }
            
            // 判断存在 & 路径逃逸
            if (access(path, F_OK) == 0 && strstr(path, "../"))
            {
                fprintf(stderr, L_SR_LINE_FAILED_PATH_ERR, count);
            }
            else
            {
                long clear_size = s_remove(path, 1); // 返回清理字节数
                if (clear_size != -1)
                {
                    fprintf(stderr, L_SR_CLEAR_SUCCESSFUL, path, (clear_size / 1024 / 1024));
                }
                else
                {
                    fprintf(stderr, L_SR_CLEAR_FAILED, path);
                }
            }
            fflush(stdout);
        }
        fclose(config_file_fp);
    }
    closedir(config_dir_dp);
    
    printf(L_SR_END);
    return 0;
}
