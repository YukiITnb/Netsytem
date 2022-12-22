#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    system("ifconfig > ifconfig.out");
    FILE* f = fopen("ifconfig.out","rb");
    fseek(f, 0, SEEK_END);
    int fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* data = (char*)calloc(fsize + 1, sizeof(char));
    fread(data, sizeof(char), fsize, f);
    fclose(f);
    char* tmp = data;
    char adapterName[1024];
    char inet[1024];
    char ip[1024];
    while (strstr(tmp, "flags=") != NULL)
    {
        tmp = strstr(tmp, "flags=");
        while (tmp[0] != '\r' && tmp[0] != '\n' && tmp != data)
        {
            tmp -= 1;
        }        
        if (tmp[0] == '\r' || tmp[0] == '\n')
            tmp += 1;        

        memset(adapterName, 0, sizeof(adapterName));
        sscanf(tmp, "%s", adapterName);  
        if (strstr(tmp, "inet ") != NULL )  
        {
            tmp = strstr(tmp, "inet ");
            memset(inet, 0, sizeof(inet));
            memset(ip, 0, sizeof(ip));
            sscanf(tmp, "%s%s", inet, ip);
            printf("%s %s\n", adapterName, ip);
        }else
            printf("%s IPv4 not available\n", adapterName);
    }
    free(data);
}
