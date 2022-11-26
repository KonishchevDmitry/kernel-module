.PHONY: build check install clean

KDIR := /lib/modules/$(shell uname -r)/build

obj-m        += my.o
EXTRA_CFLAGS += -DDEBUG -Wno-declaration-after-statement

IGNORE_LINTS := SPDX_LICENSE_TAG,BRACES,OPEN_BRACE,LINE_SPACING,POINTER_LOCATION

build:
	make -C $(KDIR) M=$(PWD) modules

check:
	cppcheck -v --force --enable=all --suppress=missingIncludeSystem .
	make C=2 CHECK=/usr/bin/sparse -C $(KDIR) M=$(PWD) modules
	make W=1 -C $(KDIR) M=$(PWD) modules
	flawfinder *.[ch]
	$(KDIR)/scripts/checkpatch.pl --no-tree --show-types --ignore "$(IGNORE_LINTS)" -f --max-line-length=95 *.[ch]

run: build
	sudo rmmod my ||:
	sudo insmod my.ko
	sudo dmesg

install: build
	sudo make -C $(KDIR) M=$(PWD) modules_install
	sudo depmod

clean:
	make -C $(KDIR) M=$(PWD) clean