# Copyright (c) 2011-2014 Joe Sylve <joe.sylve@gmail.com> (504ENSICS Labs) LiME Linux Memory Extractor
# Copyright (c) 2022 Engin Kuzu <enginkuzu@gmail.com>

obj-m := midump.o
midump-objs := disk.o main.o

KVER ?= $(shell uname -r)
KDIR ?= /lib/modules/$(KVER)/build

PWD := $(shell pwd)

SECUREBOOT := $(shell mokutil --sb-state 2>/dev/null)
SIGNFILE := /usr/src/linux-headers-$(KVER)/scripts/sign-file
KEYLOCATION := /var/lib/shim-signed/mok/

.PHONY: modules modules_install clean distclean debug

default:
	$(MAKE) -C $(KDIR) M="$(PWD)" modules
	strip --strip-unneeded midump.ko
ifeq ($(SECUREBOOT), SecureBoot enabled)
	sudo $(SIGNFILE) sha256 $(KEYLOCATION)MOK.priv $(KEYLOCATION)MOK.der midump.ko
endif
	mv -f midump.ko midump-$(KVER).ko

debug:
	KCFLAGS="-DMIDUMP_DEBUG" $(MAKE) CONFIG_DEBUG_SG=y -C $(KDIR) M="$(PWD)" modules
	strip --strip-unneeded midump.ko
ifeq ($(SECUREBOOT), SecureBoot enabled)
	sudo $(SIGNFILE) sha256 $(KEYLOCATION)MOK.priv $(KEYLOCATION)MOK.der midump.ko
endif
	mv -f midump.ko midump-$(KVER).ko

symbols:
	$(MAKE) -C $(KDIR) M="$(PWD)" modules
ifeq ($(SECUREBOOT), SecureBoot enabled)
	sudo $(SIGNFILE) sha256 $(KEYLOCATION)MOK.priv $(KEYLOCATION)MOK.der midump.ko
endif
	mv -f midump.ko midump-$(KVER).ko

modules: main.c disk.c
	$(MAKE) -C /lib/modules/$(KVER)/build M="$(PWD)" $@
	strip --strip-unneeded midump.ko
ifeq ($(SECUREBOOT), SecureBoot enabled)
	sudo $(SIGNFILE) sha256 $(KEYLOCATION)MOK.priv $(KEYLOCATION)MOK.der midump.ko
endif

modules_install: modules
	$(MAKE) -C $(KDIR) M="$(PWD)" $@

clean:
	rm -f *.o *.mod *.mod.c Module.symvers modules.order .*.cmd

distclean: mrproper
mrproper: clean
	rm -f *.ko

