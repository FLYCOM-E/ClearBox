#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define SERVER "ClearBox"

int main(int argc, char * argv[])
{
    if (argc != 3)
    {
        printf("Use: ./Post <title> <message>\n");
        return 1;
    }
    
    if (setuid(2000) == -1)
    {
        printf("Setuid Error\n");
        return 1;
    }
    
    pid_t newPid = fork();
    if (newPid == -1)
    {
        printf("Fork Error\n");
        return 1;
    }
    
    if (newPid == 0)
    {
        execlp("cmd", "cmd", "notification", "post", "-t", argv[1], SERVER, argv[2], NULL);
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
