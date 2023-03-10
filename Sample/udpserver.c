#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
int main()
{
    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    SOCKADDR_IN saddr, caddr;
    int clen = sizeof(caddr);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(6101);
    saddr.sin_addr.s_addr = 0;
    bind(s, (SOCKADDR*)&saddr, sizeof(saddr));
    while (1)
    {
        char buffer[1024] = { 0 };
        int r = recvfrom(s, buffer, sizeof(buffer), 0, (SOCKADDR*)&caddr, &clen);
        if (r > 0)
        {
            printf("%s\n", buffer);
            caddr.sin_port = htons(5000);
            sendto(s, buffer, strlen(buffer), 0, (SOCKADDR*)&caddr, sizeof(caddr));
        }else
            break;
    }
    close(s); 
}