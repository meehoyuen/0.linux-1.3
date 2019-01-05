#
# linux/arch/i386/boot/compressed/Makefile
#
# create a compressed vmlinux image from the original vmlinux
#

HEAD = head.o
SYSTEM = $(TOPDIR)/vmlinux

OBJECTS = $(HEAD) misc.o

CFLAGS = -O2 -DSTDC_HEADERS

ifdef SMP
CFLAGS := $(CFLAGS) -D__SMP__
endif

ifdef CONFIG_KERNEL_ELF
INPUT_DATA=input_data
INPUT_LEN=input_len
endif

all: vmlinux

vmlinux: piggy.o $(OBJECTS)
	$(LD) $(ZLINKFLAGS) -o vmlinux $(OBJECTS) piggy.o

bvmlinux: piggy.o $(OBJECTS)
	$(LD) $(ZLINKFLAGS) -o bvmlinux $(OBJECTS) piggy.o

ifdef SMP

head.o: head.S $(TOPDIR)/include/linux/tasks.h
	$(CC) -D__SMP__ -traditional -c head.S

else

head.o: head.S $(TOPDIR)/include/linux/tasks.h
	$(CC) -traditional -c head.S

endif

ifdef CONFIG_KERNEL_ELF

# You cannot compress a file and have the kernel uncompress it, it must
# be stdin
piggy.o:$(SYSTEM) $(ENCAPS)
	tmppiggy=/tmp/$$$$.piggy; \
	rm -f $$tmppiggy $$tmppiggy.gz $$tmppiggy.lnk; \
	$(OBJCOPY) $(SYSTEM) $$tmppiggy; \
	gzip -f -9 < $$tmppiggy > $$tmppiggy.gz; \
	echo "SECTIONS { .data : { input_len = .; LONG(input_data_end - input_data) input_data = .; *(.data) input_data_end = .; }}" > $$tmppiggy.lnk; \
	$(LD) -r -o piggy.o -b binary $$tmppiggy.gz -b elf32-i386 -T $$tmppiggy.lnk; \
	rm -f $$tmppiggy $$tmppiggy.gz $$tmppiggy.lnk
else

piggy.o:$(SYSTEM) xtract piggyback
		./xtract $(SYSTEM) | gzip -9 | ./piggyback > piggy.o

xtract: xtract.c
	$(HOSTCC) $(CFLAGS) -o xtract xtract.c

piggyback: piggyback.c
	$(HOSTCC) $(CFLAGS) -o piggyback piggyback.c

endif

clean:
	rm -f xtract piggyback vmlinux bvmlinux
