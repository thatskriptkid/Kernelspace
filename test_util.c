#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    char *buff="Hello my module!";
    
    int fd;
    
    fd=open("/dev/my_char_device",O_RDWR);
    
    printf("bytes writed = %d",(write(fd,buff,strlen(buff))));
	
	
    return 0;
    }
