ifneq ($(KERNELRELEASE),)
	obj-m := adc/ adc_test/ nxt_sense/ level_shifter/ level_shifter_test/
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
	(cd nxt_sense; make install)