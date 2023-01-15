obj-m := dma_test.o

# MY_CFLAGS += -g -DDEBUG
# ccflags-y += ${MY_CFLAGS}

SRC := $(shell pwd)
KERNEL_SRC := /home/king/linux-xlnx

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers
	rm -rf *.mod
