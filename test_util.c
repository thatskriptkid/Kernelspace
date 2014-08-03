#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
 #include <fcntl.h>

int main(){
    FILE *fd;
    fd=fopen("/dev/my_char_device","r+");

    if(fd==NULL)
    printf("fail\n");
    return 0;
    }
