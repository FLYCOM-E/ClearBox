// 此Core来自ClearBox模块，用于打开原生安卓墓碑模式（其实Shell可能更适合？
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    if (getuid() != 0)
    {
        printf(" » 请使用Root权限！\n");
        return 1;
    }
    
    int sdk = 0;
    char sdk_str[4] = "";
    FILE * sdk_fp = popen("getprop ro.build.version.sdk", "r");
    if (sdk_fp)
    {
        fgets(sdk_str, sizeof(sdk_str), sdk_fp);
        pclose(sdk_fp);
        sdk_str[strcspn(sdk_str, "\n")] = 0;
        sdk = atoi(sdk_str);
    }
    else
    {
        printf(" » 获取系统版本失败！\n");
        return 1;
    }
    
    if (sdk >= 30)
    {
        if (system("device_config put activity_manager_native_boot use_freezer true") == 0)
        {
            printf(" » 已打开安卓原生墓碑 (^^)\n");
        }
        else
        {
            printf(" » 打开安卓原生墓碑失败！\n");
        }
    }
    
    return 0;
}
