#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <malloc.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define INVALID_SOCKET -1
#define MAX_CLIENT 1024
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

int g_clientfd[MAX_CLIENT];
int g_clientcount = 0;

void signal_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        for (int i = 0;i < g_clientcount;i++)
        {
            char feedback[1024] = { 0 };
            FILE* f = fopen("data.tmp","rt");
            fgets(feedback, sizeof(feedback), f);
            fclose(f);
            send(g_clientfd[i], feedback, strlen(feedback), 0);
        }
    }else if (signum == SIGCHLD)
    {
        int stat = 0;
        while (waitpid(-1, &stat, WNOHANG) > 0)
        {
            //DO NOTHING HERE SIMPLY WAIT FOR CHILD PROCESS TO BE RELEASED
        }
    }else if (signum == SIGINT)
    {
        exit(0);
    }
}

int main()
{
    int parent_id = getpid();
    signal(SIGINT, signal_handler);
    signal(SIGCHLD, signal_handler);
    signal(SIGUSR1, signal_handler);

    int sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);    
    SOCKADDR_IN saddr, caddr;
    int clen = sizeof(caddr);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(5000);
    saddr.sin_addr.s_addr = 0;
    bind(sfd, (SOCKADDR*)&saddr, sizeof(saddr));
    listen(sfd, 10);
    while (0 == 0)
    {
        int cfd = accept(sfd, (SOCKADDR*)&caddr, &clen);
        if (fork() == 0)
        {
            close(sfd);
            while (0 == 0)
            {
                char buffer[1024] = { 0 };
                printf("Receving data...\n");
                int r = recv(cfd, buffer, sizeof(buffer), 0);
                if (r > 0)
                {
                    // printf("addr: %s",caddr);
                    printf("Received: %s\n", buffer);
                    send(cfd, buffer, strlen(buffer), 0);
                    FILE* f = fopen("data.tmp", "wt");
                    fprintf(f, "%s", buffer);
                    fclose(f);
                    kill(parent_id, SIGUSR1);
                }else
                    exit(0);
            }
        }else{
            printf("Client count: %d\n", g_clientcount + 1);
            g_clientfd[g_clientcount++] = cfd;
        }
    }

    close(sfd);
    return 0;
}