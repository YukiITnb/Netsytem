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
#include <pthread.h>

#define INVALID_SOCKET -1
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
#define MAX_CLIENT 32
int g_clientcount = 0;
int g_clientfd[MAX_CLIENT];
int sfd = 0;

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

void process(char* fn, int port){
    char buffer[1024];
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN saddr, caddr;
    int clen = sizeof(caddr);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = 0;
    bind(fd, (SOCKADDR*)&saddr, sizeof(saddr));
    listen(fd, 10);
    while (0 == 0)
    {
        int cfd = accept(fd, (SOCKADDR*)&caddr, &clen);
        FILE* f = fopen(fn, "rt");
        while (!feof(f))
        {
            char buffer[1024] = { 0 };
            fgets(buffer, sizeof(buffer), f);
            send(cfd, buffer, strlen(buffer), 0);
        }
        fclose(f);
        close(cfd);
    }
}

void senddone(){
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN saddr, caddr;
    int clen = sizeof(caddr);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(5555);
    saddr.sin_addr.s_addr = 0;
    bind(fd, (SOCKADDR*)&saddr, sizeof(saddr));
    listen(fd, 10);
    while (0 == 0)
    {
        int cfd = accept(fd, (SOCKADDR*)&caddr, &clen);
        send(cfd, "DONE", 4, 0);
        close(cfd);
    }
}
int main(){
    int parent_id = getpid();
    signal(SIGINT, signal_handler);
    signal(SIGCHLD, signal_handler);
    signal(SIGUSR1, signal_handler);
    sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN saddr, caddr;
	int clen = sizeof(caddr);
    saddr.sin_port = htons(5555);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = 0;
    int error = bind(sfd, (SOCKADDR*)&saddr, sizeof(saddr));
	if (error == 0)
	{
		listen(sfd, 10);
		while (0 == 0)		
		{   
            int tmp = accept(sfd, (SOCKADDR*)&caddr, &clen);
            if (fork() == 0){
                // char buffer[10] = "huan";
                // send(tmp, buffer, strlen(buffer), 0);
                char buffer[1024] = { 0 };
                int r = recv(tmp, buffer, sizeof(buffer), 0);
                if(r >0){
                    printf("recive: %s", buffer);
                    if (strstr(buffer, "QUIT"))
                        break;
                    else{
                        char GET[8] = {0};
                        char FILENAME[1024] = {0};
                        char PORT[16] = {0};
                        sscanf(buffer, "%s%s%s", GET, FILENAME, PORT);
                        if(strcmp(GET,"GET") == 0 && FILENAME != NULL && PORT != NULL){
                            int port = atoi(PORT);
                            process(FILENAME,port);
                            senddone();

                            close(tmp);
                        }
                        else
                            send(tmp, "invalid command", 15, 0);
                    }
                }
            }else{
                if(g_clientcount > 32){
                    break;
                }
            }
        }
            
	}else
	{
		printf("PORT is already in used\n");
		close(sfd);
	}
}