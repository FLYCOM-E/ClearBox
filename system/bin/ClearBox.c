// By ClearBox
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <dirent.h>

//home目录
#define home_dir "/data/adb/modules/wipe_cache"
//work目录
#define work_dir "/data/adb/wipe_cache"
//bin目录
#define BinDir_1 "/data/adb/magisk"
#define BinDir_2 "/data/adb/ap/bin"
#define BinDir_3 "/data/adb/ksu/bin"

static int RunService();
static int md_menu();
static int ClearCache();
static int PrintBinDir();
static int PrintHomeDir();
static int PrintWorkDir();
static int GetVersion();
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
        return PrintHomeDir();
    }
    if (strcasecmp(COM[1], "-w") == 0)
    {
        return PrintWorkDir();
    }
    if (strcasecmp(COM[1], "-b") == 0)
    {
        return PrintBinDir();
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
        return GetVersion();
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
    snprintf(command, sizeof(command), "sh '%s/service.sh' >>/dev/null 2>&1", home_dir);
    
    end = system(command);
    if (end == 0)
    {
        printf(" » Done\n");
    }
    else
    {
        printf(" » Run `service.sh` Error! \n");
        return 1;
    }
    
    return 0;
}

//此函数用于启动模块终端UI脚本
static int md_menu()
{
    int end = 0;
    char command[256] = "";
    snprintf(command, sizeof(command), "bash '%s/Menu.sh'", home_dir);
    
    end = system(command);
    if (end != 0)
    {
        printf(" » Start Menu Error! \n");
        return 1;
    }
    
    return 0;
}

//此函数用于清理操作
static int ClearCache()
{
    int end = 0;
    char command[256] = "";
    snprintf(command, sizeof(command), "%s/BashCore ClearAll", home_dir);
    
    end = system(command);
    if (end != 0)
    {
        printf(" » Clear Error! \n");
        return 1;
    }
    
    return 0;
}

//此函数用于检查root方案并返回对应busybox路径
static int PrintBinDir()
{
    if (access(BinDir_1, F_OK) == 0)
    {
        printf("%s\n", BinDir_1);
        return 0;
    }
    else if (access(BinDir_2, F_OK) == 0)
    {
        printf("%s\n", BinDir_2);
        return 0;
    }
    else if (access(BinDir_3, F_OK) == 0)
    {
        printf("%s\n", BinDir_3);
        return 0;
    }
    else
    {
        return 1;
    }
    
    return 0;
}

//此函数用于输出home路径
static int PrintHomeDir()
{
    printf("%s\n", home_dir);
    return 0;
}

//此函数用于输出work路径
static int PrintWorkDir()
{
    printf("%s\n", work_dir);
    return 0;
}

//此函数用于获取模块版本
static int GetVersion()
{
    char * var_fp = NULL;
    char var[64] = "", module_file[64] = "";
    snprintf(module_file, sizeof(module_file), "%s/module.prop", home_dir);
    
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
    else
    {
        printf("Get Version Error !\n");
        return 1;
    }
    
    return 0;
}

//help
static int md_help()
{
    printf("\nUsage: ClearBox\n");
    printf("Or ClearBox -[OPTION]:\n");
    printf("\t-u\n");
    printf("\t\t~ Update Profile\n");
    printf("\t-c\n");
    printf("\t\t~ Run Clear All\n");
    printf("\t-v\n");
    printf("\t\t~ Version\n");
    printf("\t-b\n");
    printf("\t\t~ Root Manager Busybox Bin Dir\n");
    printf("\t-h\n");
    printf("\t\t~ ClearBox Module Home Dir\n");
    printf("\t-w\n");
    printf("\t\t~ ClearBox Module Work Dir\n");
    printf("\t-help\n");
    printf("\t\t~ Help\n\n");
    
    return 0;
}
