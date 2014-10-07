obj-m := SSL_client.o

ccflags-y := -I$(src)/include -g3 -Wall -msse -mmmx 

SSL_client-y := SSL_client.o lib/ksocket.o lib/klog.o   

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
