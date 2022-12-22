#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <malloc.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char** argv)
{
    if (argc > 1)
    {
        char* domain = argv[1];
        //char* domain = "vnexpress.net";
        struct hostent* presult = gethostbyname(domain);
        if (presult != NULL)
        {
            struct hostent host = *presult;
            int type = host.h_addrtype;
            int count = 0;
            while (host.h_addr_list[count] != NULL)
            {
                char* addr = host.h_addr_list[count];
                struct in_addr inaddr;
                memcpy(&inaddr, addr, sizeof(inaddr));
                char* saddr = inet_ntoa(inaddr);
                printf("%s\n", saddr);
                count += 1;
            }
        }
    }
}