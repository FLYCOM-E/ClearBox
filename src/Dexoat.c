/*
                    GNU GENERAL PUBLIC
                        Version 3

      此 Code 来自 ClearBox 模块，用于运行 Dexoat 编译
*/

#include "INCLUDE/main.h"

static void dexoat_system(void);
static int dexoat_cust(char * mode);

int dexoat(int mode, char * cust_mode)
{
    // 触发系统 Dexoat
    if (mode == 0)
    {
        dexoat_system();
    }
    else if (mode == 1)
    {
        return dexoat_cust(cust_mode);
    }
    
    return 0;
}

// 系统Dexoat
static void dexoat_system(void)
{
    fprintf(stderr, L_DO_RUN_SYSTEM);
    system("pm bg-dexopt-job");
    return;
}

// 自定义模式Dexoat
static int dexoat_cust(char * mode)
{
    fprintf(stderr, L_DO_RUN_CUST, mode);
    
    pid_t newPid = fork();
    if (newPid == -1)
    {
        return -1;
    }
    if (newPid == 0)
    {
        execlp("pm", "pm", "compile", "-m", mode, "-f", "-a", NULL);
        _exit(127);
    }
    else
    {
        int end = 0;
        if (waitpid(newPid, &end, 0) == -1)
        {
            return -1;
        }
        if (WIFEXITED(end) == 0 && WEXITSTATUS(end) != 0)
        {
            return -1;
        }
    }
    return 0;
}
