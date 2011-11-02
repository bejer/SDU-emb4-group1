ifneq ($(KERNELRELEASE),)
	obj-m := adc/ adc_test/
else
	PWD := $(shell pwd)

default:
ifeq ($(strip $(KERNELDIR)),)
	$(error "KERNELDIR is undefined!")
else
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif
endif

install:
	(cd adc; make install)
	(cd adc_test; make install)