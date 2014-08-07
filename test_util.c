#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    char *buff="Hello my module!";
    char income_data[16];//magic number
    int fd,i;
    ssize_t count;
    
    fd=open("/dev/my_char_device",O_RDWR);
    
    count=write(fd,buff,strlen(buff));
	printf("bytes writed = %d\n",count);
	
	count=read(fd,income_data,16);
	printf("bytes readed = %d\n",count);
	
	for(i=0;i<16;i++)
		printf("%c",income_data[i]);

    return 0;
    }
