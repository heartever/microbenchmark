EXTRA_CFLAGS = -m64
obj-m += disablecache.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
