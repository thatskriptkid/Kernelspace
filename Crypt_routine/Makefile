obj-m := cr.o

ccflags-y := -I$(src)/include -g3 -Wall 

cr-y := crypt.o lib/klog.o lib/entropy_poll.o lib/platform.o lib/aes.o lib/ctr_drbg.o lib/entropy.o lib/sha256.o lib/sha512.o  

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
