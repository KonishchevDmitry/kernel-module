.PHONY: build check install clean

KDIR := /lib/modules/$(shell uname -r)/build

obj-m          += my.o
EXTRA_CFLAGS   += -DDEBUG

build:
	make -C $(KDIR) M=$(PWD) modules

check:
	cppcheck -v --force --enable=all --suppress=missingIncludeSystem .
	make C=2 CHECK=/usr/bin/sparse -C $(KDIR) M=$(PWD) modules
	make W=1 -C $(KDIR) M=$(PWD) modules
	flawfinder *.[ch]

	indent -linux --line-length95 *.[chsS]
	$(KDIR)/scripts/checkpatch.pl --no-tree --show-types -f --max-line-length=95 *.[ch]

	make clean

run: build
	sudo rmmod my ||:
	sudo insmod my.ko
	sudo dmesg

install: build
	sudo make -C $(KDIR) M=$(PWD) modules_install
	sudo depmod

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f *~  # from indent