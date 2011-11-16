# MACHINE=overo
# OETMP=/home/michael/embedded/emb4/overo-oe-guide/build/tmp
# SYSROOTSDIR=${OETMP}/sysroots
# STAGEDIR=${SYSROOTSDIR}/`uname -m`-linux/usr

# KERNELDIR=${SYSROOTSDIR}/${MACHINE}-angstrom-linux-gnueabi/kernel

# export PATH=:${PATH}:${STAGEDIR}/bin:${STAGEDIR}/armv7a/bin

# #unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS MACHINE

# export ARCH="arm"
# export CROSS_COMPILE="arm-angstrom-linux-gnueabi-"
# export CC="arm-angstrom-linux-gnueabi-gcc"
# export LD="arm-angstrom-linux-gnueabi-ld"
# export STRIP="arm-angstrom-linux-gnueabi-strip"



# #KERNELDIR = /home/michael/embedded/emb4/overo-oe-guide/build/tmp/work/overo-angstrom-linux-gnueabi/linux-omap3-2.6.36-r100/git
export = /home/michael/Temp/SDU-emb4-group1
# cross-compile module makefile
NAME := leddev

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

.PHONY: clean
clean:
	-rm $(NAME).o $(NAME).ko $(NAME).mod.c $(NAME).mod.o .$(NAME).mod.o.cmd .$(NAME).ko.cmd modules.order Module.symvers
	-rm -r .tmp_versions

endif
