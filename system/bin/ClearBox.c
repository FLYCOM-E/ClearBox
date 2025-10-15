// By ClearBox
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

char home[256] = "/data/adb/modules/wipe_cache";

int RunService();
int md_menu();
int ClearCache();
int bin_dir();
int home_dir();
int work_dir();
int version();
int md_help();

int main(int COMI, char * COM[])
{
    uid_t nowuid = getuid();
    if (nowuid != 0)
    {
        printf(" » Please use root privileges!\n");
        return 0;
    }
    
    if (COMI < 2)
    {
        md_menu();
        return 0;
    }
    if (strcasecmp(COM[1], "-h") == 0)
    {
        home_dir();
        return 0;
    }
    if (strcasecmp(COM[1], "-w") == 0)
    {
        work_dir();
        return 0;
    }
    if (strcasecmp(COM[1], "-b") == 0)
    {
        bin_dir();
        return 0;
    }
    if (strcasecmp(COM[1], "-u") == 0)
    {
        RunService();
        return 0;
    }
    if (strcasecmp(COM[1], "-c") == 0)
    {
        ClearCache();
        return 0;
    }
    if (strcasecmp(COM[1], "-v") == 0 ||
       strcasecmp(COM[1], "--version") == 0)
    {
        version();
        return 0;
    }
    if (strcasecmp(COM[1], "-help") == 0 ||
       strcasecmp(COM[1], "--help") == 0)
    {
        md_help();
        return 0;
    }
    else
    {
        md_help();
        return 0;
    }
    
    return 0;
}

int RunService()
{
    char shname[] = "/service.sh";
    char command[256] = "sh ";
    int end;
    
    strcat(command, home);
    strcat(command, shname);
    
    end = system(command);
    
    if (end == 0)
    {
        printf(" » Done\n");
    }
    else
    {
        printf(" » Run Error! \n");
    }
    
    return 0;
}

int md_menu()
{
    char shname[] = "/Menu.sh";
    char command[256] = "bash ";
    int end;
    
    strcat(command, home);
    strcat(command, shname);
    
    end = system(command);
    
    if (end != 0)
    {
        printf(" » Run Error! \n");
    }
    
    return 0;
}

int ClearCache()
{
    char shname[] = "/all.sh ClearAll";
    char command[256] = "bash ";
    int end;
    
    strcat(command, home);
    strcat(command, shname);
    
    end = system(command);
    
    if (end != 0)
    {
        printf(" » Run Error! \n");
    }
    
    return 0;
}

int bin_dir()
{
    DIR * dir = NULL;
    
    dir = opendir("/data/adb/magisk");
    if (dir != NULL)
    {
        printf("/data/adb/magisk\n");
        closedir(dir);
    }
    
    dir = opendir("/data/adb/ap");
    if (dir != NULL)
    {
        printf("/data/adb/ap/bin\n");
        closedir(dir);
    }
    
    dir = opendir("/data/adb/ksu");
    if (dir != NULL)
    {
        printf("/data/adb/ksu/bin\n");
        closedir(dir);
    }
    
    return 0;
}

int home_dir()
{
    printf("%s\n", home);
    
    return 0;
}

int work_dir()
{
    printf("/data/adb/wipe_cache\n");
    
    return 0;
}

int version()
{
    char command[256] = "grep 'version=' ";
    char filename[24] = "/module.prop ";
    char getversion[48] = "| cut -f2 -d '='";
    
    strcat(command, home);
    strcat(command, filename);
    strcat(command, getversion);
    
    printf("ClearBox Version: ");
    fflush(stdout);
    system(command);
    
    return 0;
}

int md_help()
{
    printf("                             \n");
    printf("Usage: ClearBox\n");
    printf("                             \n");
    printf("or ClearBox [ OPTION ]:\n");
    printf("                             \n");
    printf("    -u\n");
    printf("           ~ Update Profile\n");
    printf("                             \n");
    printf("    -c\n");
    printf("           ~ Clear All\n");
    printf("                             \n");
    printf("    -v\n");
    printf("           ~ Version\n");
    printf("                             \n");
    printf("    -b\n");
    printf("           ~ Busybox Bin Dir\n");
    printf("                             \n");
    printf("    -h\n");
    printf("           ~ Module Home Dir\n");
    printf("                             \n");
    printf("    -w\n");
    printf("           ~ Module Work Dir\n");
    printf("                             \n");
    printf("    --help\n");
    printf("           ~ Module Help\n");
    printf("                             \n");
    
    return 0;
}
