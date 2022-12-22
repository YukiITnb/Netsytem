#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <malloc.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define INVALID_SOCKET -1
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

int sendPacket(int fd, char* data, int len){
    int sent = 0;
    do{
        sent += send(fd ,data + sent, len - sent, 0); 
    }while(sent<len);

    return sent;
}

int recivePacket(int fd, char* data, int maxlen){
    int recived = 0;
    int blocksize = 2;
    int tmp = 0;
    do{
        tmp = recv(fd, data + recived, blocksize, 0);
        recived += tmp;
    }while (recived >= 0 && recived < maxlen && tmp == blocksize);
}

int main(int argc, char** argv)
{
    int sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sfd != INVALID_SOCKET)
    {
        SOCKADDR_IN saddr, caddr;
        int clen = sizeof(caddr);
        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(5555);
        saddr.sin_addr.s_addr = 0;
        bind(sfd, (SOCKADDR*)&saddr, sizeof(saddr));
        listen(sfd, 10); 
        int cfd = accept(sfd, (SOCKADDR*)&caddr, &clen);
        if (cfd != INVALID_SOCKET)
        {
            // char* welcome = "Hello my first TCP server\n";
            char* text;
            scanf("%s",text);
            send(cfd, text, strlen(text), 0);
            while (0 == 0)
            {
                char buffer[1024] = { 0 };
                recv(cfd, buffer, sizeof(buffer), 0);
                system("%s > out.text", buffer);
                FILE* fp = fopen("out.text","rt");

                fclose(fp);
                printf("%s\n", buffer);
                if (strncmp(buffer, "exit", 4) == 0)
                {
                    break;
                }
            }
            close(cfd);
        }
        close(sfd);
    }
}