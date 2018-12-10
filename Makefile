KDIR= /lib/modules/$(shell uname -r)/build

obj-m := aaudidev.o

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

install:
	$(MAKE) -C $(KDIR) M=$(PWD) modules_install


clean:
	rm -f *~
	$(MAKE) -C $(KDIR) M=$(PWD) clean
