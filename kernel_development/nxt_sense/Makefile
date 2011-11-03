# cross-compile module makefile
export := /home/michael/Temp/SDU-emb4-group1
NAME := nxt_sense_mod

ifneq ($(KERNELRELEASE),)
	obj-m := nxt_sense_mod.o
	nxt_sense_mod-objs := nxt_sense.o touch.o
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



