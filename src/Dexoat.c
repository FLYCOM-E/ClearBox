// 此Code来自ClearBox模块，用于运行DexOat
#include "INCLUDE/BashCore.h"

static int SystemDexOat();
static int CustDexOat(char * mode);

int main(int argc, char * argv[])
{
    if (getuid() != 0)
    {
        fprintf(stderr, L_NOT_USE_ROOT);
        return 1;
    }
    if (argc < 2)
    {
        fprintf(stderr, L_ARGS_FAILED);
        return 1;
    }
    
    if (strcasecmp(argv[1], "SYSTEM_DEXOAT") == 0)
    {
        return SystemDexOat();
    }
    else if (strcasecmp(argv[1], "FAST_DEXOAT") == 0)
    {
        if (argc < 3)
        {
            fprintf(stderr, L_ARGS_FAILED);
            return 1;
        }
        return CustDexOat(argv[2]);
    }
    else
    {
        fprintf(stderr, L_ARGS_FAILED_2);
        return 1;
    }
    
    return 0;
}

// 系统Dexoat
static int SystemDexOat()
{
    printf(L_DO_RUN_SYSTEM);
    fflush(stdout);
    return system("cmd package bg-dexopt-job");
}

// 自定义模式Dexoat
static int CustDexOat(char * mode)
{
    printf(L_DO_RUN_CUST, mode);
    fflush(stdout);
    
    pid_t newPid = fork();
    if (newPid == -1)
    {
        return 1;
    }
    if (newPid == 0)
    {
        execlp("cmd", "cmd", "package", "compile", "-m", mode, "-f", "-a", NULL);
        _exit(127);
    }
    else
    {
        int end = 0;
        if (waitpid(newPid, &end, 0) == -1)
        {
            return 1;
        }
        if (WIFEXITED(end) == 0 && WEXITSTATUS(end) != 0)
        {
            return 1;
        }
    }
    
    return 0;
}
