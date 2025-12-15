// 此Code来自ClearBox模块，用于运行DexOat
#include "BashCore.h"

static int SystemDexOat();
static int CustDexOat(char * mode);

int main(int COMI, char * COM[])
{
    if (getuid() != 0)
    {
        printf(L_NOT_USE_ROOT);
        return 1;
    }
    if (COMI < 2)
    {
        printf(L_ARGS_FAILED);
        return 1;
    }
    
    if (strcasecmp(COM[1], "SYSTEM_DEXOAT") == 0)
    {
        return SystemDexOat();
    }
    else if (strcasecmp(COM[1], "FAST_DEXOAT") == 0)
    {
        if (COMI < 3)
        {
            printf(L_ARGS_FAILED);
            return 1;
        }
        return CustDexOat(COM[2]);
    }
    else
    {
        printf(L_ARGS_FAILED_2);
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
    char command[strlen(mode) + 64];
    snprintf(command, sizeof(command), "cmd package compile -m %s -f -a", mode);
    return system(command);
}
