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
	cp leddev.ko $(export)/export/own_modules

clean:
	-rm *.o *.mod.c modules.order Module.symvers .leddev* .tmp_versions

endif