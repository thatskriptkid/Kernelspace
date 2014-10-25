obj-m := ssl.o

ccflags-y := -I$(src)/include -mpreferred-stack-boundary=4 -Wall -msse -mmmx 

ssl-y := SSL_client.o lib/bignum.o lib/aesni.o lib/sha256.o lib/platform.o lib/threading.o lib/ssl_cli.o lib/cipher.o lib/ssl_tls.o lib/aesni.o lib/sha256.o lib/aes.o lib/entropy.o lib/entropy_poll.o lib/ctr_drbg.o lib/klog.o lib/ksocket.o  

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
