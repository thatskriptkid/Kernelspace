obj-m=crypt.o

ccflags-y := -I$(src)/include -g3 -Wall -Werror

crypt-y := crypt.o lib/aes.o 

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
