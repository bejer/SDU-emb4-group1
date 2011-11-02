# cross-compile module makefile
export := /home/michael/Temp/SDU-emb4-group1
NAME := nxt_sense

ifneq ($(KERNELRELEASE),)
	obj-m := $(NAME).o
	$(NAME)-objs := touch.o
else
    PWD := $(shell pwd)

default:
ifeq ($(strip $(KERNELDIR)),)
	$(error "KERNELDIR is undefined!")
else
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

install:
	cp $(NAME).ko $(export)/export/own_modules

clean:
	-rm *.o *.mod.c modules.order Module.symvers .$(NAME)* .tmp_versions

endif



