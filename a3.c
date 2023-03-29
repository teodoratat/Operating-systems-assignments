#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
int fdMyPipe, fdYourPipe;
char textSize;
char text[60];
char *aux1,*aux2;
char *data,*data1;
unsigned int nrBytess,offset,value;
int connectPipes()
{
    // int mknod(const char *path_name, mode_t acc_rights_and_type, dev_t disp);
    // int mkfifo(const char *path_name, mode_t acc_rights);
    int ok = 0;
    if (mkfifo("RESP_PIPE_22972", 0644) == 0)
    {
        printf("pipe created successfully\n");
        ok = 1;
    }
    fdYourPipe = open("REQ_PIPE_22972", O_RDONLY);
    if (fdYourPipe == -1)
    {

        perror("Error opening your pipe\n");
        exit(1);
    }
    else
    {
        printf("success opening your file\n");
        ok++;
    }
    fdMyPipe = open("RESP_PIPE_22972", O_WRONLY);
    if (fdMyPipe == -1)
    {

        perror("Error opening my pipe\n");

        exit(2);
    }
    else
    {
        printf("success opening my file\n");
        ok++;
    }
    textSize = 7;
    write(fdMyPipe, &textSize, 1);
    if (write(fdMyPipe, "CONNECT", strlen("CONNECT")) == -1)
    {
        perror("fail on writing\n");
    }
    else
    {
        printf("success writing in my file\n");
        ok++;
    }
    return ok;
}
void printSuccess(char *string)
{
    textSize = strlen(string);
    write(fdMyPipe, &textSize, 1);
    write(fdMyPipe, string, textSize);
    textSize = 7;
    write(fdMyPipe, &textSize, 1);
    write(fdMyPipe, "SUCCESS", textSize);
}
void printError(char *string)
{
    textSize = strlen(string);
    write(fdMyPipe, &textSize, 1);
    write(fdMyPipe, string, textSize);
    textSize = 5;
    write(fdMyPipe, &textSize, 1);
    write(fdMyPipe, "ERROR", textSize);
}
void pingPong()
{
    textSize = 4;
    write(fdMyPipe, &textSize, 1);
    write(fdMyPipe, "PING", strlen("PING"));
    textSize = 4;
    write(fdMyPipe, &textSize, 1);
    write(fdMyPipe, "PONG", strlen("PONG"));
    // 22972
    textSize = 5;
    unsigned int myNumber = 22972;
    // write(fdMyPipe, &textSize, 1);
    write(fdMyPipe, &myNumber, 4);
}
void createSharedMem(char *text, unsigned int nrBytes)
{
    //int shm_open(const char *name, int oflag, mode_t mode);
    //int shm_unlink(const char *name);
    //int ftruncate(int fd, off_t length);
    int shm = shm_open("/Xh1VLa", O_CREAT | O_RDWR, 0664);
    ftruncate(shm, nrBytes);
    if (shm < 0)
    {
        printError(text);
    }
    else
    {
        data = (char*)mmap(0, nrBytes, PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
        if (data == MAP_FAILED)
        {
            printError(text);
        }
        else
            printSuccess(text);
    }
}
void writeToMem(char *text, unsigned int offset, unsigned int value,unsigned int nrBytes)
{
    if(offset + 4 > nrBytes)
    {
        printError(text);
    }
    else
    {
        *(unsigned int*)(data + offset) = value;
        printSuccess(text);
    }
}
void mapMemory(char *text, char *fileName)
{

    int shm = open(fileName, O_RDWR);
    int size = lseek(shm, 0, SEEK_END);
    if (shm < 0)
    {
        printError(text);
    }
    else
    {
        data1 = (char*)mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
        if (data1 == MAP_FAILED)
        {
            printError(text);
        }
        else
            printSuccess(text);
    }
    printError(text);
}
void fileOffsetReq(char *text, unsigned int offset, unsigned int nrOfBytes)
{
    printError(text);
}
void readSF(char *text, int nrSect, unsigned int offset, unsigned int nrOfBytes)
{
    printError(text);
}
void readSfLogicMem(char *text, unsigned int offset, unsigned int nrOfBytes)
{
    printError(text);
}
void exitFromConnection()
{
    close(fdMyPipe);
    close(fdYourPipe);
    unlink("RESP_PIPE_22972");
}
int main(int argc, char **argv)
{
    if (connectPipes() == 4)
    {
        printf("SUCCESS");
    }
    else
    {
        printf("ERROR\n");
        printf("cannot create the response pipe | cannot open the request pipe");
    }
    while (1)
    {
        read(fdYourPipe, &textSize, 1);
        read(fdYourPipe, text, textSize);
        if (strcmp(text, "PING") == 0)
            pingPong();
        if (strstr(text, "CREATE_SHM") == text)
        {
            read(fdYourPipe,&nrBytess,4);
            createSharedMem(text,nrBytess);
        }
        else if(strstr(text, "WRITE_TO_SHM") == text)
        {
            read(fdYourPipe,&offset,4);
            read(fdYourPipe,&value,4);
            writeToMem(text,offset,value,nrBytess);

        }
        else if(strstr(text, "MAP_FILE") == text)
        {
            char fileName[2000];
            char fNameLen;
            read(fdYourPipe, &fNameLen, 1);
            read(fdYourPipe,fileName, fNameLen);
            mapMemory(text,fileName);

        }
        else
        {
            exitFromConnection();
            break;
        }
    }
}