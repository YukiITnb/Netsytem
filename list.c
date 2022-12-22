// #include <stdio.h>
// #include <malloc.h>
// #include <string.h>
// #include <stdlib.h>

// int main()
// {
//     system("ls -a -l > ls.out");
//     FILE* f = fopen("ls.out","rb");
//     fseek(f, 0, SEEK_END);
//     int fsize = ftell(f);
//     fseek(f, 0, SEEK_SET);
//     char* data = (char*)calloc(fsize + 1, sizeof(char));
//     fread(data, sizeof(char), fsize, f);
//     fclose(f);
//     char* tmp = data;
//     char fileName[1024];
//     char filesize[1024];
//     while (strstr(tmp, "yuki yuki") != NULL)
//     {
//         tmp = strstr(tmp, "yuki yuki");
//         tmp += 8;
//         while (tmp[0] == ' ')
//         {
//             tmp += 1;
//         }
        
//         while (tmp[0] != ' ' && tmp != data)
//         {
//             tmp += 1;
//         }        
        
//         memset(filesize, 0 , sizeof(filesize));
//         sscanf(tmp, "%s", filesize);
//         // printf("%s\n", filesize);

//         if(strstr(tmp, "\n") != NULL){
//             tmp = strstr(tmp, "\n");
//         }

//         while (tmp[0] != ' ' && tmp != data)
//         {
//             tmp -= 1;
//         }        
        
//         memset(fileName, 0 , sizeof(fileName));
//         sscanf(tmp, "%s", fileName);
//         printf("%10s %s\n", fileName, filesize);
//     }
//     free(data);
// } chi chay duoc tren may cua ban than

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    system("ls -a -l > ls.out");
    FILE* f = fopen("ls.out","rt");
    char line[1024];
    while (!feof(f))
    {
        memset(line, 0, sizeof(line));
        fgets(line, sizeof(line), f);
        if (line[0] == '-')
        {
            char per[1024] = { 0 };
            char v1[1024] = { 0 };
            char group[1024] = { 0 };
            char owner[1024] = { 0 };
            char size[1024] = {0 } ;
            char month[1024] = { 0 };
            char v2[1024] = { 0 };
            char time[1024] = { 0 };
            sscanf(line, "%s%s%s%s%s%s%s%s", per, v1, group, owner, size, month, v2, time);
            char* name = strstr(line, time) + strlen(time) + 1;
            while ( name[strlen(name) - 1] == '\n' || 
                    name[strlen(name) - 1] == '\r')
                    name[strlen(name) - 1] = 0;
            printf("%s %s\n", name, size);
        }
    }
    fclose(f);
}
