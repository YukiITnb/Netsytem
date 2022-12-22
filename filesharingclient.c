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

#define MAX 100
#define PORT 5000
#define SA struct sockaddr
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

void sighandler(int signum)
{
    int stat = 0;
    while (waitpid(-1, &stat, WNOHANG) > 0);
}


void udp_pro(){
    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    SOCKADDR_IN saddr, ackaddr;
    ackaddr.sin_family = AF_INET;
    ackaddr.sin_port = htons(7000);
    ackaddr.sin_addr.s_addr = 0;
    bind(s, (SOCKADDR*)&ackaddr, sizeof(ackaddr));

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(4000);
    saddr.sin_addr.s_addr = inet_addr("255.255.255.255");
    char buffer[1024] = { 0 };
    printf("Enter the client name : ");
    int n = 0;
    while ((buffer[n++] = getchar()) != '\n')
        ;
    sendto(s, buffer, strlen(buffer), 0, (SOCKADDR*)&saddr, sizeof(saddr));

    close(s);
}
void tcp_pro(){
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN saddr, caddr;
    int clen = sizeof(caddr);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(5000);
    saddr.sin_addr.s_addr = 0;
    connect(fd,((SOCKADDR*)&saddr), sizeof(saddr));
}

int main(){
    signal(SIGCHLD, sighandler);
    if (fork() == 0)
    {
        udp_process();
    }
    if (fork() == 0)
    {
        tcp_process();
    }
    while (0 == 0)
    {
        sleep(1000);
    }    

    return 0;
}