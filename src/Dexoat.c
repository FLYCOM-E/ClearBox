// 此Core来自ClearBox模块，用于运行DexOat
#include "BashCore.h"

static int SystemDexOat();
static int CustDexOat(char * mode);

int main(int COMI, char * COM[])
{
    if (getuid() != 0)
    {
        printf(" » 请授予root权限！\n");
        return 1;
    }
    if (COMI < 2)
    {
        printf(" » 参数不足！\n");
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
            printf(" » 需要一个参数，未传入模式！\n");
            return 1;
        }
        return CustDexOat(COM[2]);
    }
    else
    {
        printf(" » 参数错误！\n");
        return 1;
    }
    
    return 0;
}

// 系统Dexoat
static int SystemDexOat()
{
    printf(" » 正在进行系统默认 Dexoat...\n");
    fflush(stdout);
    return system("cmd package bg-dexopt-job");
}

// 自定义模式Dexoat
static int CustDexOat(char * mode)
{
    printf(" » 正在运行模式 %s 编译...\n", mode);
    fflush(stdout);
    char command[strlen(mode) + 64];
    snprintf(command, sizeof(command), "cmd package compile -m %s -f -a", mode);
    return system(command);
}
