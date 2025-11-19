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

static int updateConfig();
static int ttyMenu();
static int clearCache();
static int printBinDir();
static int printHomeDir();
static int printWorkDir();
static int getVersion();
static int printHelp();

int main(int COMI, char * COM[])
{
    if (getuid() != 0)
    {
        printf(" » Please use root privileges!\n");
        return 1;
    }
    
    //定义调用
    if (COMI < 2 ||
       strcasecmp(COM[1], "-m") == 0)
    {
        return ttyMenu();
    }
    if (strcasecmp(COM[1], "-h") == 0)
    {
        return printHomeDir();
    }
    if (strcasecmp(COM[1], "-w") == 0)
    {
        return printWorkDir();
    }
    if (strcasecmp(COM[1], "-b") == 0)
    {
        return printBinDir();
    }
    if (strcasecmp(COM[1], "-u") == 0)
    {
        return updateConfig();
    }
    if (strcasecmp(COM[1], "-c") == 0)
    {
        return clearCache();
    }
    if (strcasecmp(COM[1], "-v") == 0 ||
       strcasecmp(COM[1], "-version") == 0 ||
       strcasecmp(COM[1], "--version") == 0)
    {
        return getVersion();
    }
    if (strcasecmp(COM[1], "-help") == 0 ||
       strcasecmp(COM[1], "--help") == 0 ||
       strcasecmp(COM[1], "help") == 0)
    {
        return printHelp();
    }
    else
    {
        printHelp();
        return 1;
    }
    
    return 0;
}

// 通用函数，用于运行脚本
static int Run(char * bash, char * str)
{
    char * command_args[4] = {NULL};
    if (strstr(bash, ".sh"))
    {
        command_args[0] = "bash";
        command_args[1] = bash;
        command_args[2] = str;
        command_args[3] = NULL;
    }
    else
    {
        command_args[0] = bash;
        command_args[1] = str;
        command_args[2] = NULL;
    }
    
    pid_t newPid = fork();
    if (newPid == -1)
    {
        return 1;
    }
    if (newPid == 0)
    {
        execvp(command_args[0], command_args);
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
static int updateConfig()
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
static int ttyMenu()
{
    char bash[128] = "";
    snprintf(bash, sizeof(bash), "%s/Menu.sh", home_dir);
    if (Run(bash, "") != 0)
    {
        printf(" » Start Menu error\n");
        return 1;
    }
    
    return 0;
}

//此函数用于清理操作
static int clearCache()
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
static int printBinDir()
{
    if (access(BinDir_1, F_OK) == 0)
    {
        printf("%s\n", BinDir_1);
    }
    else if (access(BinDir_2, F_OK) == 0)
    {
        printf("%s\n", BinDir_2);
    }
    else if (access(BinDir_3, F_OK) == 0)
    {
        printf("%s\n", BinDir_3);
    }
    else
    {
        return 1;
    }
    
    return 0;
}

//此函数用于输出home路径
static int printHomeDir()
{
    printf("%s\n", home_dir);
    return 0;
}

//此函数用于输出work路径
static int printWorkDir()
{
    printf("%s\n", work_dir);
    return 0;
}

//此函数用于获取模块版本
static int getVersion()
{
    char * key_p = NULL;
    char module_file_line[64] = "", module_file[64] = "";
    snprintf(module_file, sizeof(module_file), "%s/module.prop", home_dir);
    
    FILE * module_file_fp = fopen(module_file, "r");
    if (module_file_fp)
    {
        while (fgets(module_file_line, sizeof(module_file_line), module_file_fp))
        {
            if (strstr(module_file_line, "version=") == NULL)
            {
                continue;
            }
            key_p = strtok(module_file_line, "=");
            key_p = strtok(NULL, "=");
            if (key_p)
            {
                printf("ClearBox Version: %s", key_p);
                fclose(module_file_fp);
                return 0;
            }
        }
    }
    
    fclose(module_file_fp);
    printf("Get Version Error !\n");
    return 1;
}

//help
static int printHelp()
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
