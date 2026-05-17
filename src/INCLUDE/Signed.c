#include "BashCore.h"

volatile sig_atomic_t sig_flag = 1;
static void s_signed_service(int sig);

/*
信号注册处理
依赖：
    s_signed_service() 函数，进行信号处理
返回：
    成功返回 0，失败返回 1
*/
int s_signed(void)
{
    struct sigaction sg;
    sg.sa_handler = s_signed_service;
    sigemptyset(&sg.sa_mask);
    sg.sa_flags = 0;
    
    if (sigaction(SIGINT, &sg, NULL) == -1)
    {
        return 1;
    }
    if (sigaction(SIGTERM, &sg, NULL) == -1)
    {
        return 1;
    }
    if (sigaction(SIGQUIT, &sg, NULL) == -1)
    {
        return 1;
    }
    
    return 0;
}

/*
信号处理辅助函数，设置全局标志位
*/
static void s_signed_service(int sig)
{
    (void)sig;
    sig_flag = 0;
}
