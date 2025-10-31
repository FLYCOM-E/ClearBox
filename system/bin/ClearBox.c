// By ClearBox
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <dirent.h>

//home目录
char home[256] = "/data/adb/modules/wipe_cache";

static int RunService();
static int md_menu();
static int ClearCache();
static int bin_dir();
static int home_dir();
static int work_dir();
static int version();
static int md_help();

int main(int COMI, char * COM[])
{
    uid_t nowuid = getuid();
    if (nowuid != 0)
    {
        printf(" » Please use root privileges!\n");
        return 1;
    }
    
    //定义调用
    if (COMI < 2)
    {
        return md_menu();
    }
    if (strcasecmp(COM[1], "-h") == 0)
    {
        return home_dir();
    }
    if (strcasecmp(COM[1], "-w") == 0)
    {
        return work_dir();
    }
    if (strcasecmp(COM[1], "-b") == 0)
    {
        return bin_dir();
    }
    if (strcasecmp(COM[1], "-u") == 0)
    {
        return RunService();
    }
    if (strcasecmp(COM[1], "-c") == 0)
    {
        return ClearCache();
    }
    if (strcasecmp(COM[1], "-v") == 0 ||
       strcasecmp(COM[1], "-version") == 0 ||
       strcasecmp(COM[1], "--version") == 0)
    {
        return version();
    }
    if (strcasecmp(COM[1], "-help") == 0 ||
       strcasecmp(COM[1], "--help") == 0 ||
       strcasecmp(COM[1], "help") == 0)
    {
        return md_help();
    }
    else
    {
        md_help();
        return 1;
    }
    
    return 0;
}

//此函数用于更新运行模块server
static int RunService()
{
    int end = 0;
    char command[256] = "";
    snprintf(command, sizeof(command), "sh '%s/service.sh' >>/dev/null 2>&1", home);
    
    end = system(command);
    if (end == 0)
    {
        printf(" » Done\n");
    }
    else
    {
        printf(" » Run Error! \n");
        return 1;
    }
    
    return 0;
}

//此函数用于启动模块终端UI脚本
static int md_menu()
{
    int end = 0;
    char command[256] = "";
    snprintf(command, sizeof(command), "bash '%s/Menu.sh'", home);
    
    end = system(command);
    if (end != 0)
    {
        printf(" » Run Error! \n");
        return 1;
    }
    
    return 0;
}

//此函数用于清理操作
static int ClearCache()
{
    int end = 0;
    char command[256] = "";
    snprintf(command, sizeof(command), "bash '%s/all.sh' ClearAll", home);
    
    end = system(command);
    if (end != 0)
    {
        printf(" » Run Error! \n");
        return 1;
    }
    
    return 0;
}

//此函数用于检查root方案并返回对应busybox路径
static int bin_dir()
{
    DIR * dir_1 = opendir("/data/adb/magisk");
    DIR * dir_2 = opendir("/data/adb/ap");
    DIR * dir_3 = opendir("/data/adb/ksu");
    
    if (dir_1 != NULL)
    {
        printf("/data/adb/magisk\n");
        closedir(dir_1);
        return 0;
    }
    else if (dir_2 != NULL)
    {
        printf("/data/adb/ap/bin\n");
        closedir(dir_2);
        return 0;
    }
    else if (dir_3 != NULL)
    {
        printf("/data/adb/ksu/bin\n");
        closedir(dir_3);
        return 0;
    }
    else
    {
        return 1;
    }
    
    return 0;
}

//此函数用于输出home路径
static int home_dir()
{
    printf("%s\n", home);
    return 0;
}

//此函数用于输出work路径
static int work_dir()
{
    printf("/data/adb/wipe_cache\n");
    return 0;
}

//此函数用于获取模块版本
static int version()
{
    char * var_fp = NULL;
    char var[64] = "", module_file[64] = "";
    snprintf(module_file, sizeof(module_file), "%s/module.prop", home);
    FILE * module_file_fp = fopen(module_file, "r");
    
    if (module_file_fp)
    {
        while (fgets(var, sizeof(var), module_file_fp))
        {
            if (strstr(var, "version="))
            {
                var_fp = strtok(var, "=");
                var_fp = strtok(NULL, "=");
                if (var_fp)
                {
                    printf("ClearBox Version: %s", var_fp);
                }
            }
        }
        fclose(module_file_fp);
    }
    
    return 0;
}

//help
static int md_help()
{
    printf("\nUsage: ClearBox\n");
    printf("or ClearBox [ OPTION ]:\n");
    printf("    -u\n");
    printf("           ~ Update Profile\n");
    printf("    -c\n");
    printf("           ~ Run Clear All\n");
    printf("    -v\n");
    printf("           ~ Version\n");
    printf("    -b\n");
    printf("           ~ Root Manager Busybox Bin Dir\n");
    printf("    -h\n");
    printf("           ~ ClearBox Module Home Dir\n");
    printf("    -w\n");
    printf("           ~ ClearBox Module Work Dir\n");
    printf("    -help\n");
    printf("           ~ Help\n\n");
    
    return 0;
}
