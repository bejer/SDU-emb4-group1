# cross-compile module makefile

ifneq ($(KERNELRELEASE),)
    obj-m := leddev.o
else
    PWD := $(shell pwd)

default:
ifeq ($(strip $(KERNELDIR)),)
	$(error "KERNELDIR is undefined!")
else
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

install:
	cp leddev.ko $(EMB4ROOT)/export/own_modules

clean:
	-rm *.o *.mod.c modules.order Module.symvers .leddev* .tmp_versions

endif



