obj-m += hello.o
CURRENT_PATH:=$(shell pwd)  
KERNEL_DIR := /lib/modules/$(shell uname -r)/build
all:  
	make -C $(KERNEL_DIR) M=$(CURRENT_PATH) modules
clean:  
	make -C $(KERNEL_DIR) M=$(CURRENT_PATH) clean  

