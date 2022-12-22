#include<stdio.h>
#include<dirent.h>
int main(){
    struct dirent** output = NULL;
    int n = scandir("/home/", &output, NULL, NULL);

    FILE *f = fopen("scan.html","w");
    fputs("<html>\n",f);
    if(output != NULL){
        for(int i = 0; i < n; i++){
            switch(output[i]->d_type){
                case DT_REG:
                    printf("file %s\n", output[i]->d_name);
                    fputs("<i>",f);
                    fputs(output[i]->d_name,f);
                    fputs("</i>",f);
                    fputs("<br>\n",f);
			        break;
                case DT_DIR:
                    printf("dir %s/\n", output[i]->d_name);
                    fputs("<b>",f);
                    fputs(output[i]->d_name,f);
                    fputs("</b>",f);
                    fputs("<br>\n",f);
                    break;
                default:
			        printf("%s*\n", output[i]->d_name);
            }
        }
    }
    fputs("</html>\n",f);
    fclose(f);
}