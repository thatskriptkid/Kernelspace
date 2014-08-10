obj-m:=block_dev.o
all:
	make -C /usr/src/linux-3.15.6/ M=$(PWD) modules
clean:
	make -C /usr/src/linux-3.15.6/ M=$(PWD) clean
