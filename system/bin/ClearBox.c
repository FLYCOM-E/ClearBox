// By ClearBox
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <dirent.h>
#include <sys/wait.h>

//home目录
#define home_dir "/data/adb/modules/wipe_cache"
//work目录
#define work_dir "/data/adb/wipe_cache"
//bin目录
#define BinDir_1 "/data/adb/magisk"
#define BinDir_2 "/data/adb/ap/bin"
#define BinDir_3 "/data/adb/ksu/bin"

static int Run(char * bash, char * str);

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
    if (COMI < 2 ||
       strcasecmp(COM[1], "-m") == 0)
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

// 通用函数，用于运行脚本
static int Run(char * bash, char * str)
{
    char * args[4] = {NULL};
    if (strstr(bash, ".sh"))
    {
        args[0] = "bash";
        args[1] = bash;
        args[2] = str;
        args[3] = NULL;
    }
    else
    {
        args[0] = bash;
        args[1] = str;
        args[2] = NULL;
    }
    
    pid_t newPid = fork();
    if (newPid == -1)
    {
        return 1;
    }
    if (newPid == 0)
    {
        execvp(args[0], args);
        _exit(1);
    }
    else
    {
        int end = 0;
        if (waitpid(newPid, &end, 0) == -1)
        {
            return 1;
        }
        if (WIFEXITED(end) && WEXITSTATUS(end) != 0)
        {
            return 1;
        }
    }
    
    return 0;
}

//此函数用于更新运行模块server
static int RunService()
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/service.sh", home_dir);
    
    if (Run(bash, "") == 0)
    {
        printf(" » Done\n");
    }
    else
    {
        printf(" » Run Service error\n");
        return 1;
    }
    
    return 0;
}

//此函数用于启动模块终端UI脚本
static int md_menu()
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/Menu.sh", home_dir);
    
    if (Run(bash, "") == 0)
    {
        printf(" » Bye！欢迎再来❤️❤️\n");
    }
    else
    {
        printf(" » Start Menu error\n");
        return 1;
    }
    
    return 0;
}

//此函数用于清理操作
static int ClearCache()
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/BashCore", home_dir);
    
    if (Run(bash, "ClearAll") != 0)
    {
        printf(" » Start Clear Error! \n");
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
                    fclose(module_file_fp);
                    return 0;
                }
            }
        }
    }
    
    fclose(module_file_fp);
    printf("Get Version Error !\n");
    return 1;
}

//help
static int md_help()
{
    printf("\nUsage: ClearBox\n");
    printf("Or ClearBox -[OPTION]:\n");
    printf("\t-m\n");
    printf("\t\t~ Screen Menu\n");
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
