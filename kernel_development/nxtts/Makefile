# cross-compile module makefile
export = /home/michael/Temp/SDU-emb4-group1

ifneq ($(KERNELRELEASE),)
    obj-m := nxtts.o
else
    PWD := $(shell pwd)

default:
ifeq ($(strip $(KERNELDIR)),)
	$(error "KERNELDIR is undefined!")
else
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

install:
	cp nxtts.ko $(export)/export/own_modules

clean:
	-rm *.o *.mod.c modules.order Module.symvers .nxtts* .tmp_versions

endif



