obj-m := ssl.o

ccflags-y := -I$(src)/include -g3 -Wall -msse -mmmx 

ssl-y := SSL_client.o lib/entropy.o lib/entropy_poll.o lib/ctr_drbg.o  lib/platform.o lib/ksocket.o lib/klog.o   

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
