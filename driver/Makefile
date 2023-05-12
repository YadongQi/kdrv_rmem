#KDIR = /home/yadong/rpmbuild/BUILD/kernel-5.15.0
KVER = $(shell uname -r)
KDIR = /lib/modules/$(KVER)/build

kbuild:
	make -C $(KDIR) M=`pwd`

clean:
	make -C $(KDIR) M=`pwd` clean
