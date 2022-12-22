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

#define MAX_CONN_NUM 1024
#define INVALID_SOCKET -1
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
#define MAX_CLIENT 1024
int g_cfd[MAX_CLIENT] = { 0 };
int g_count = 0;
int sfd = 0;

void Append(char** pdst, const char* src)
{
    char* dst = *pdst; //Xau ky tu cu
    int oldLen = (dst == NULL ? 0 : strlen(dst));
    int newLen = oldLen + strlen(src) + 1;
    dst = (char*)realloc(dst, newLen);
    memset(dst + oldLen, 0, strlen(src) + 1);
    sprintf(dst + oldLen, "%s", src);
    *pdst = dst;
}

int Compare(const struct dirent** a, const struct dirent** b)
{
    if ((*a)->d_type == (*b)->d_type)
        return 0;
    else if ((*a)->d_type == DT_DIR)
        return -1;
    else
        return 1;
}

const char *get_filename_ext(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename)
        return "";
    return dot + 1;
}

void* ClientThread(void* arg)
{
	int cfd = *((int*)arg);
    free(arg);
    arg = NULL;
    char buffer[1024] = { 0 };
    int r = recv(cfd, buffer, sizeof(buffer), 0);
    if (r > 0)
    {
        char* rootPath = (char*)calloc(7, sizeof(char));
        strcpy(rootPath, "/mnt/e");
        char GET[8] = { 0 };
        char PATH[1024] = { 0 };
        sscanf(buffer, "%s%s", GET, PATH);
        while (strstr(PATH, "%20") != NULL)
        {
            char* tmp = strstr(PATH, "%20");
            tmp[0] = ' ';
            strcpy(tmp + 1, tmp + 3);
        }
        Append(&rootPath, (char*)PATH);
        if (PATH[strlen(PATH) - 1] == '/')
        {
            char* ok = "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: text/html\r\n\r\n";
            struct dirent** output = NULL;
            char* html = NULL;
            Append(&html,"<html>");
            int n = scandir(rootPath, &output, NULL, Compare);
            if (n > 0)
            {
                for (int i = 0;i < n;i++)
                {
                    char line[2048] = { 0 };
                    if (output[i]->d_type == DT_DIR)
                    {
                        if (PATH[strlen(PATH) - 1] == '/')
                            sprintf(line, "<a href = \"%s%s/\"><b>%s</b></a>", PATH, output[i]->d_name, output[i]->d_name);
                        else
                            sprintf(line, "<a href = \"%s/%s/\"><b>%s</b></a>", PATH, output[i]->d_name, output[i]->d_name);
                    }
                    if (output[i]->d_type == DT_REG)
                    {
                        if (PATH[strlen(PATH) - 1] == '/')
                            sprintf(line, "<a href = \"%s%s\"><i>%s</i></a>", PATH, output[i]->d_name, output[i]->d_name);
                        else
                            sprintf(line, "<a href = \"%s/%s\"><i>%s</i></a>", PATH, output[i]->d_name, output[i]->d_name);
                    }
                    Append(&html, line);
                    Append(&html, "<br>");
                }

                for (int i = 0;i < n;i++)
                {
                    free(output[i]);
                    output[i] = NULL;
                }
                free(output);
                output = NULL;
            }else
                Append(&html, (char*)"EMPTY FOLDER<br>");

            Append(&html, "</html>");
            char header[1024] = { 0 };
            sprintf(header, ok, strlen(html));
            send(cfd, header, strlen(header), 0);
            send(cfd, html, strlen(html), 0);
            free(html);
            html = NULL;   
        }else{
            char* ok = NULL;
            if (strcmp(get_filename_ext(rootPath), "c") == 0)
            {
                ok = "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: text/plain\r\n\r\n";
            }
            else if (strcmp(get_filename_ext(rootPath), "png") == 0)
            {
                ok = "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: image/png\r\n\r\n";
            }
            else if (strcmp(get_filename_ext(rootPath), "jpeg") == 0)
            {
                ok = "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: image/png\r\n\r\n";
            }
            else if (strcmp(get_filename_ext(rootPath), "pdf") == 0)
            {
                ok = "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: application/pdf\r\n\r\n";
            }
            else if (strcmp(get_filename_ext(rootPath), "mp4") == 0)
            {
                ok = "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: video/mp4\r\n\r\n";
            }
            else
                ok = "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: application/octet-stream\r\n\r\n";

            FILE* f = fopen(rootPath, "rb");
            if (f != NULL)
            {
                fseek(f, 0, SEEK_END);
                int fsize = ftell(f);
                fseek(f, 0, SEEK_SET);
                char header[1024] = { 0 };
                sprintf(header, ok, fsize);
                send(cfd, header, strlen(header), 0);
                char* data = (char*)calloc(fsize, 1);
                fread(data, 1, fsize, f);
                int sent = 0;
                while (sent < fsize)
                {
                    int s = send(cfd, data + sent, fsize - sent, 0);
                    if (s <= 0)
                        break;
                    else
                        sent += s;
                }
                fclose(f);
                free(data);
                data = NULL;
            }else
            {
                if (strstr(rootPath, "favicon"))
                {
                    char* ok = "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: application/octet-stream\r\n\r\n";
                    FILE* f = fopen("/mnt/e/WSL/favicon.ico","rb");
                    if (f != NULL)
                    {
                        fseek(f, 0, SEEK_END);
                        int fsize = ftell(f);
                        fseek(f, 0, SEEK_SET);
                        char header[1024] = { 0 };
                        sprintf(header, ok, fsize);
                        send(cfd, header, strlen(header), 0);
                        char* data = (char*)calloc(fsize, 1);
                        fread(data, 1, fsize, f);
                        send(cfd, data, fsize, 0);
                        fclose(f);
                        free(data);
                        data = NULL;
                    }
                }else
                {
                    char* ok = "HTTP/1.1 404 NOT FOUND\r\nContent-Length: 0\r\nContent-Type: text/html\r\n\r\n";
                    send(cfd, ok, strlen(ok), 0);
                }
            }
        }
        free(rootPath);
        rootPath = NULL;
    }

	close(cfd);
	return NULL;
}
int main()
{
    pthread_t pid;
    sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN saddr, caddr;
	int clen = sizeof(caddr);
    saddr.sin_port = htons(8888);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = 0;
    int error = bind(sfd, (SOCKADDR*)&saddr, sizeof(saddr));
	if (error == 0)
	{
		listen(sfd, 10);
		while (0 == 0)		
		{
			int tmp = accept(sfd, (SOCKADDR*)&caddr, &clen);
			g_cfd[g_count++] = tmp;
			int* arg = (int*)calloc(1, sizeof(int));
			*arg = tmp;
			pthread_create(&pid, NULL, ClientThread, (void*)arg);
		}
	}else
	{
		printf("PORT is already in used\n");
		close(sfd);
	}
}