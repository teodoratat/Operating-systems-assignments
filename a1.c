#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdbool.h>
#include <libgen.h>
#define MAX_PATH_LEN 256
/**
 * @brief
 *
 * @param dirName directory path
 * @param functions functionalities bitmask (1 - recursive, 2 - size, 4 - string, 8 - SUCCES)
 * @param size size smaller (filter)
 * @param string given substring
 *  */

int fileSize(char *path)
{
    int fd = open(path, O_RDONLY);
    int size;
    if (fd == -1)
        return 0;
    size = lseek(fd, 0, SEEK_END);
    close(fd);
    return size;
}

void listDir(char *dirName, char functions, int size, char *string)
{
    DIR *dir;
    struct dirent *dirEntry;
    struct stat inode;
    char path[MAX_PATH_LEN * 2];
    dir = opendir(dirName);
    if (dir == 0)
    {
        printf("ERROR\ninvalid directory path");
        closedir(dir);
        exit(4);
    }

    if (functions & 8)
    {
        printf("SUCCESS\n");
    }
    while ((dirEntry = readdir(dir)) != 0)
    {
        if (strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name, "..") != 0)
        {
            sprintf(path, "%s/%s", dirName, dirEntry->d_name);
            lstat(path, &inode);
            if ((functions & 1) != 0)
            {
                if (dirEntry->d_type == DT_DIR)
                {
                    listDir(path, functions & ~8, size, string);
                }
            }
            if ((functions & 2) != 0)
            {
                if ((fileSize(path) < size) && (dirEntry->d_type != DT_DIR))
                {
                    if ((functions & 4) != 0)
                    {
                        if (strstr(dirEntry->d_name, string) == dirEntry->d_name)
                        {
                            printf("%s\n", path);
                        }
                    }
                    else
                    {
                        printf("%s\n", path);
                    }
                }
            }
            else if ((functions & 4) != 0)
            {
                if (strstr(dirEntry->d_name, string) == dirEntry->d_name)
                {
                    printf("%s\n", path);
                }
            }
            else
            {
                printf("%s\n", path);
            }
        }
    }
    closedir(dir);
}
struct Header
{
    char magic[3];
    int header_size;
    int version;
    int no_of_sections;
    struct Section
    {
        char name[11];
        int type;
        int offset;
        int size;
    } *sections;
} h;
int parse(char *a,int printFlag)
{
    int fd = open(a, O_RDONLY);
    lseek(fd, 0, SEEK_SET);
    read(fd, h.magic, 2);
    h.magic[2] = 0;
    if (strcmp(h.magic, "QF") != 0)
    {
        if(printFlag)
            printf("ERROR\nwrong magic\n");
        close(fd);
        return 0;
    }
    h.header_size=0;
    h.version=0;
    read(fd, &h.header_size, 2);
    read(fd, &h.version, 2);
    if (h.version < 42 || h.version > 154)
    {
        if(printFlag)
            printf("ERROR\nwrong version\n");
        close(fd);
        return 0;
    }
    h.no_of_sections=0;
    read(fd, &h.no_of_sections, 1);
    h.sections=realloc(h.sections,h.no_of_sections*sizeof(struct Section));
    if (h.no_of_sections < 6 || h.no_of_sections > 15)
    {
        if(printFlag)
            printf("ERROR\nwrong sect_nr\n");
        close(fd);
        return 0;
    }
    for(int i=0;i<h.no_of_sections;i++)
    {
        h.sections[i].type=0;
        h.sections[i].offset=0;
        h.sections[i].size=0;
        read(fd, h.sections[i].name, 10);
        h.sections[i].name[10]=0;
        read(fd, &h.sections[i].type, 4);
        read(fd, &h.sections[i].offset, 4);
        read(fd, &h.sections[i].size, 4);
        if (h.sections[i].type != 57 && h.sections[i].type != 42 && h.sections[i].type != 96 && h.sections[i].type != 98)
        {
            if(printFlag)
                printf("ERROR\nwrong sect_types\n");
            close(fd);
            return 0;
        }
    }
    if(printFlag)
    {
        printf("SUCCESS\n");
        printf("version=%d\n", h.version);
        printf("nr_sections=%d\n", h.no_of_sections);
        for (int j = 0; j < h.no_of_sections; j++)
        {
            printf("section%d: ", j + 1);
            printf("%s %d %d\n", h.sections[j].name, h.sections[j].type, h.sections[j].size);
        }
    }
    close(fd);
    return 1;
}

void extract(char *path,int sect_nr, int line_nr)
{
    int fd=open(path,O_RDONLY);
    if(fd==-1||parse(path,0)==0)
    {
        printf("ERROR\ninvalid file\n");
            close(fd);
        return;
    }
    
    if(sect_nr<1||sect_nr>h.no_of_sections)
    {
        printf("ERROR\ninvalid section\n");
        close(fd);
        return;   
    }

    char* allSection=(char*)calloc(sizeof(char),h.sections[sect_nr-1].size+1);
    char* allSectionAux=(char*)calloc(sizeof(char),h.sections[sect_nr-1].size+1);
    lseek(fd,h.sections[sect_nr-1].offset,SEEK_SET);
    read(fd,allSection,h.sections[sect_nr-1].size);
    strcpy(allSectionAux,allSection);

    int nrLines=0;
    char*p=strtok(allSectionAux,"\r\n");
    while(p!=NULL){
        nrLines++;
        p=strtok(NULL,"\r\n");
    }

    p=strtok(allSection,"\r\n");
    while(p!=NULL)
    {
        if(nrLines==line_nr){
            printf("SUCCESS\n%s\n",p);
            free(allSection);
            free(allSectionAux);
            return;
        }
        nrLines--;
        p=strtok(NULL,"\r\n");
    }

    printf("\nERROR\ninvalid line");
    free(allSectionAux);
    free(allSection);
}
void findall(char *dirPath, char functions)
{

    DIR *dir;
    struct dirent *dirEntry;
    struct stat inode;
    char path[MAX_PATH_LEN * 2];
    dir = opendir(dirPath);
    if (dir == 0)
    {
        printf("ERROR\ninvalid directory path");
        return;
    }
    if (functions & 8)
    {
        printf("SUCCESS\n");
    }
    while ((dirEntry = readdir(dir)) != 0)
    {
        if (strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name, "..") != 0)
        {
            sprintf(path, "%s/%s", dirPath, dirEntry->d_name);
            lstat(path, &inode);
                if (dirEntry->d_type == DT_DIR)
                {
                    findall(path,functions & ~8);
                }
                else
                {
                    if(parse(path,0)!=1){
                        closedir(dir);
                        return;
                    }
                    int nrSectVerified=0;
                    for(int i=0;i<h.no_of_sections;i++)
                    {
                        if(h.sections[i].type==98)
                        {
                            nrSectVerified++;
                        }

                    }
                    if(nrSectVerified>=2)
                        printf("%s\n", path);
                }
                
        } 
}
closedir(dir);
}
int main(int argc, char **argv)
{
    if (argc == 2)
    {
        if (strcmp(argv[1], "variant") == 0)
        {
            printf("22972\n");
        }
    }
    else if (argc >= 3)
    {
        h.sections=calloc(sizeof(struct Section),1);
        char function = 8;
        char path[MAX_PATH_LEN], *sz, *aux;
        int size = -1;
        char string[MAX_PATH_LEN];
        int op = 0;
        int section=0,line=0;
        for (int i = 0; i < argc; i++)
            if ((strcmp(argv[i], "list") == 0))
            {
                op = 1;
            }
            else if ((strcmp(argv[i], "parse") == 0))
            {
                op = 2;
            }
            else if ((strcmp(argv[i], "extract") == 0))
            {
                op = 3;
            }
            else if ((strcmp(argv[i], "findall") == 0))
            {
                op = 4;
            }
            else if ((strstr(argv[i], "path=") == argv[i]))
            {
                aux = strtok(argv[i], "=");
                aux = strtok(NULL, "=");
                sprintf(path, "%s", aux);
            }
            else if ((strstr(argv[i], "size_smaller") == argv[i]))
            {
                sz = strtok(argv[i], "=");
                sz = strtok(NULL, "=");
                size = atoi(sz);
                function |= 2;
            }
            else if ((strstr(argv[i], "name_starts_with") == argv[i]))
            {
                aux = strtok(argv[i], "=");
                aux = strtok(NULL, "=");
                sprintf(string, "%s", aux);
                function |= 4;
            }
            else if ((strcmp(argv[i], "recursive")) == 0)
            {
                function |= 1;
            }
            else if((strstr(argv[i], "section=") == argv[i])){
                aux = strtok(argv[i], "=");
                aux = strtok(NULL, "=");
                section = atoi(aux);
            }
            else if((strstr(argv[i], "line=") == argv[i])){
                aux = strtok(argv[i], "=");
                aux = strtok(NULL, "=");
                line = atoi(aux);
            }
        if (op == 1)
        {
            listDir(path, function, size, string);
        }
        else if (op == 2)
        {
            parse(path,1);
        }
        else if (op == 3)
        {
            extract(path,section,line);
        }
        else if (op == 4)
        {
            findall(path,function);
        }
        free(h.sections);
    }

    return 0;
}