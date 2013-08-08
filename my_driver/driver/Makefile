#Makefile for building my kernel driver to learn Kernel Driver development

#KERNEL=/usr/src/linux-ami
#KERNEL = /usr/src/kernels/snb-linux
KERNEL = /usr/src/kernels/snb_os
#KERNEL = /usr/src/linux-ami

obj-m:= mydriver.o
mydriver-objs := myd_init.o
mydriver-objs += myd_ioctl.o
mydriver-objs += myd_device.o
mydriver-objs += myd_proc.o
mydriver-objs += myd_mem.o
mydriver-objs += myd_ioctl_hlpr.o

INCLUDE_DIRS += -I $(PWD)/../include/
EXTRA_CFLAGS += $(INCLUDE_DIRS)
EXTRA_CFLAGS += -DOWNER

all:
	@echo "*************Creating mydriver.ko..***************"
	$(MAKE) -C $(KERNEL) SUBDIRS=$(PWD) modules
clean:
	@echo "************Cleaning..**************"
	@rm -f ./*.o*
	@rm -f ./*.mod.*
	@rm -f ./*.ko*
	@rm -f ./Module*
