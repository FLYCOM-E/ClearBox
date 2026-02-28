// 此Code来自ClearBox模块，用于运行DexOat
#include "INCLUDE/BashCore.h"

static int dexoat_system(void);
static int dexoat_cust(char * mode);

int dexoat(int mode, char * cust_mode)
{
    if (mode == 0)
    {
        return dexoat_system();
    }
    else if (mode == 1)
    {
        return dexoat_cust(cust_mode);
    }
    
    return 0;
}

// 系统Dexoat
static int dexoat_system(void)
{
    fprintf(stderr, L_DO_RUN_SYSTEM);
    fflush(stdout);
    return system("cmd package bg-dexopt-job");
}

// 自定义模式Dexoat
static int dexoat_cust(char * mode)
{
    fprintf(stderr, L_DO_RUN_CUST, mode);
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
