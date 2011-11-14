# cross-compile module makefile
NAME := voltage_sensor

ifneq ($(KERNELRELEASE),)
    obj-m := $(NAME).o
else
    PWD := $(shell pwd)

default:
ifeq ($(strip $(KERNELDIR)),)
	$(error "KERNELDIR is undefined!")
else
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

install:
	cp $(NAME).ko $(EMB4ROOT)/export/own_modules

clean:
	-rm *.o *.mod.c modules.order Module.symvers .$(NAME)* .tmp_versions

endif



