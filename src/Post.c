#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char * argv[])
{
    if (argc != 3)
    {
        printf("Use: ./Post <title> <message>\n");
        return 1;
    }
    
    //生成一个随机ID
    srand(time(NULL));
    char rand_str[24] = {0};
    snprintf(rand_str, sizeof(rand_str), "%s-%d", argv[1], rand());
    
    pid_t newPid = fork();
    if (newPid == -1)
    {
        printf("Fork Error\n");
        return 1;
    }
    
    if (newPid == 0)
    {
        setuid(2000);
        execlp("cmd", "cmd", "notification", "post", "-t", argv[1], rand_str, argv[2], NULL);
        _exit(127);
    }
    else
    {
        int end = 0;
        if (waitpid(newPid, &end, 0) == -1)
        {
            printf("Wait Error\n");
            return 1;
        }
        if (WIFEXITED(end) && WEXITSTATUS(end) != 0)
        {
            printf("exit code error\n");
        }
    }
}
