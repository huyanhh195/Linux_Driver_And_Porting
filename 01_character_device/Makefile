obj-m += mgpio.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all: clean
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean	
	clear