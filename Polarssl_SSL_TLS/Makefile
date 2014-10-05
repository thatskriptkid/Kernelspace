obj-m := SSL_client.o

ccflags-y := -I$(src)/include -g3 -Wall -msse -mmmx 

cr-y := SSL_client.o lib/klog.o lib/net.o  

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
