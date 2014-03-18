# The make utility automatically determines which pieces of a large program 
# need to be recompiled, and issues commands to recompile them. This file tells
# make how to compile and link the program. This makefile also tells make how 
# to run miscellaneous commands when explicitly asked (for example, to run an
# emulator after building the correct filesystem image, or to remove certain 
# files as a clean-up operation). 
#
# http://www.gnu.org/software/make/manual/


################################################################################
# Objects
################################################################################

# kernel objects
OBJS = \
	bio.o\
	console.o\
	exec.o\
	file.o\
	fs.o\
	ide.o\
	ioapic.o\
	kalloc.o\
	kbd.o\
	lapic.o\
	main.o\
	mp.o\
	picirq.o\
	pipe.o\
	proc.o\
	spinlock.o\
	string.o\
	swtch.o\
	syscall.o\
	sysfile.o\
	sysproc.o\
	timer.o\
	trapasm.o\
	trap.o\
	uart.o\
	vectors.o\
	vm.o

# user library files
ULIB = \
	ulib.o\
	usys.o\
	printf.o\
	umalloc.o

# user programs
UPROGS = \
	cat\
	echo\
	forktest\
	grep\
	init\
	kill\
	ln\
	ls\
	mkdir\
	rm\
	sh\
	stressfs\
	tester\
	usertests\
	wc\
	zombie\
	stat

UOBJS = $(UPROGS:%=%.o)

DEPS = $(OBJS:.o=.d) $(ULIB:.o=.d) $(UOBJS:.o=.d)

################################################################################
# Compiler Options
################################################################################

# Cross-compiling (e.g., on Mac OS X)
#TOOLPREFIX = i386-jos-elf-

# Using native tools (e.g., on X86 Linux)
#TOOLPREFIX = 

# Try to infer the correct TOOLPREFIX if not set
ifndef TOOLPREFIX
TOOLPREFIX := $(shell if i386-jos-elf-objdump -i 2>&1 | grep '^elf32-i386$$' >/dev/null 2>&1; \
	then echo 'i386-jos-elf-'; \
	elif objdump -i 2>&1 | grep 'elf32-i386' >/dev/null 2>&1; \
	then echo ''; \
	else echo "***" 1>&2; \
	echo "*** Error: Couldn't find an i386-*-elf version of GCC/binutils." 1>&2; \
	echo "*** Is the directory with i386-jos-elf-gcc in your PATH?" 1>&2; \
	echo "*** If your i386-*-elf toolchain is installed with a command" 1>&2; \
	echo "*** prefix other than 'i386-jos-elf-', set your TOOLPREFIX" 1>&2; \
	echo "*** environment variable to that prefix and run 'make' again." 1>&2; \
	echo "*** To turn off this error, run 'gmake TOOLPREFIX= ...'." 1>&2; \
	echo "***" 1>&2; exit 1; fi)
endif

# C compiler options
# http://gcc.gnu.org/onlinedocs/gcc-4.4.6/gcc/Invoking-GCC.html
CC = $(TOOLPREFIX)gcc
CFLAGS =

# disable PIC (position independent code)
CFLAGS += -fno-pic

# do not use GCC builtin funtions (used to optimize common functions)
CFLAGS += -fno-builtin

# disable some optimizations that assume one object will not alias an object 
#of a different type
CFLAGS += -fno-strict-aliasing

# disable stack smashing check
CFLAGS += -fno-stack-protector

# uncomment to enable optimizations. improves performance, but may make 
# debugging more difficult
#CFLAGS += -O2

# enable extra warnings
CFLAGS += -Wall
# treat warnings as errors
CFLAGS += -Werror
# generate code for 32-bit environment
CFLAGS += -m32
# produce debugging information for use by gdb
CFLAGS += -ggdb


# Assembler options
# http://sourceware.org/binutils/docs/as/Invoking.html
AS = $(TOOLPREFIX)gcc
ASFLAGS = -m32 # generate code for 32-bit environment
ASFLAGS += -ggdb # produce debugging information for use by gdb


# Linker options
# http://sourceware.org/binutils/docs/ld/Options.html
LD = $(TOOLPREFIX)ld

# FreeBSD ld wants ``elf_i386_fbsd''
LDFLAGS += -m $(shell $(LD) -V | grep elf_i386 2>/dev/null)

# linkner flags for xv6 user programs
# use simple contiguous section layout and do not use dynamic linking
LDUSER += --omagic
# where program execution should begin
LDUSER += --entry=main
# location in memory where the program will be loaded
LDUSER += --section-start=.text=0x0


OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump


################################################################################
# Emulator Options
################################################################################

# If the makefile can't find QEMU, specify its path here
QEMU = ~cs537-1/ta/tools/qemu

# Try to infer the correct QEMU if not specified
ifndef QEMU
QEMU = $(shell if which qemu > /dev/null; \
	then echo qemu; exit; \
	else \
	qemu=/Applications/Q.app/Contents/MacOS/i386-softmmu.app/Contents/MacOS/i386-softmmu; \
	if test -x $$qemu; then echo $$qemu; exit; fi; fi; \
	echo "***" 1>&2; \
	echo "*** Error: Couldn't find a working QEMU executable." 1>&2; \
	echo "*** Is the directory containing the qemu binary in your " 1>&2; \
	echo "*** PATH or have you tried setting the QEMU variable in " 1>&2; \
	echo "*** Makefile?" 1>&2; \
	echo "***" 1>&2; exit 1)
endif

# try to generate a unique GDB port
GDBPORT = $(shell expr `id -u` % 5000 + 25000)

# QEMU's gdb stub command line changed in 0.11
QEMUGDB = $(shell if $(QEMU) -help | grep -q '^-gdb'; \
	then echo "-gdb tcp::$(GDBPORT)"; \
	else echo "-s -p $(GDBPORT)"; fi)

# number of CPUs to emulate in QEMU
ifndef CPUS
CPUS := 2
endif

QEMUOPTS = -hdb fs.img xv6.img -smp $(CPUS)


################################################################################
# Main Targets
################################################################################

.PHONY: all clean distclean tags dvi html pdf ps \
	run qemu qemu-nox qemu-gdb qemu-nox-gdb bochs depend

all: xv6.img fs.img

clean: 
	rm -f *.tex *.dvi *.idx *.aux *.log *.ind *.ilg *.d \
	*.o *.asm *.sym vectors.S parport.out \
	bootblock bootother bootother.out initcode initcode.out kernel \
	xv6.img fs.img mkfs
	rm -rf fs
	rm -rf fmt

distclean: clean
	rm -f TAGS xv6.pdf xv6.ps .gdbinit .bochsrc

tags: TAGS
TAGS: $(OBJS) bootother.S init.o
	etags *.S *.c

dvi:
html:
pdf: xv6.pdf
ps: xv6.ps

run: qemu

qemu: fs.img xv6.img
	$(QEMU) -serial mon:stdio $(QEMUOPTS)

qemu-nox: fs.img xv6.img
	$(QEMU) -nographic $(QEMUOPTS)

qemu-gdb: fs.img xv6.img .gdbinit
	@echo "*** Now run 'gdb'." 1>&2
	$(QEMU) -serial mon:stdio $(QEMUOPTS) -S $(QEMUGDB)

qemu-nox-gdb: fs.img xv6.img .gdbinit
	@echo "*** Now run 'gdb'." 1>&2
	$(QEMU) -nographic $(QEMUOPTS) -S $(QEMUGDB)

bochs: fs.img xv6.img .bochsrc
	bochs -q

depend: $(DEPS)

################################################################################
# Build Recipies
################################################################################

# include dependency files (make will automatically build them if they are 
# out of date)
-include $(DEPS)

xv6.img: bootblock kernel
	dd if=/dev/zero of=xv6.img count=10000
	dd if=bootblock of=xv6.img conv=notrunc
	dd if=kernel of=xv6.img seek=1 conv=notrunc

bootblock: bootasm.S bootmain.c
	$(CC) $(CFLAGS) -fno-pic -O -nostdinc -I. -c bootmain.c
	$(CC) $(CFLAGS) -fno-pic -nostdinc -I. -c bootasm.S
	$(LD) $(LDFLAGS) --omagic --entry=start --section-start=.text=0x7C00 \
		-o bootblock.o bootasm.o bootmain.o
	$(OBJCOPY) -S -O binary -j .text bootblock.o bootblock
	./sign.pl bootblock

bootother: bootother.S
	$(CC) $(CFLAGS) -nostdinc -I. -c bootother.S
	$(LD) $(LDFLAGS) --omagic --entry=start --section-start=.text=0x7000 \
		-o bootother.out bootother.o
	$(OBJCOPY) -S -O binary bootother.out bootother

initcode: initcode.S
	$(CC) $(CFLAGS) -nostdinc -I. -c initcode.S
	$(LD) $(LDFLAGS) --omagic --entry=start --section-start=.text=0x0 \
		--output=initcode.out initcode.o
	$(OBJCOPY) -S -O binary initcode.out initcode

kernel: $(OBJS) multiboot.o data.o bootother initcode
	$(LD) $(LDFLAGS) --section-start=.text=0x100000 --entry=main -o kernel \
		multiboot.o data.o $(OBJS) -b binary initcode bootother

vectors.S: vectors.pl
	perl vectors.pl > vectors.S

# default recipe for object files
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# default recipes for dependency files
%.d: %.c
	$(CC) $(CFLAGS) -M -MG $< -MF $@ -MT $@ -MT $(<:.c=.o)

%.d: %.S
	$(AS) $(ASFLAGS) -M -MG $< -MF $@ -MT $@ -MT $(<:.S=.o)

# user programs
fs/%: %.o $(ULIB)
	@mkdir -p fs
	$(LD) $(LDFLAGS) $(LDUSER) --output=$@ $< $(ULIB)

fs/%: %
	@mkdir -p fs
	cp $< $@

%.asm: %.o
	$(OBJDUMP) -S $< > $@

%.sym: %.o
	$(OBJDUMP) -t $< | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $@

# forktest has less library code linked in - needs to be small
# in order to be able to max out the proc table.
fs/forktest: forktest.o $(ULIB)
	@mkdir -p fs
	$(LD) $(LDFLAGS) $(LDUSER) --output=$@ forktest.o ulib.o usys.o

# mkfs is run on the host machine, not inside the emulator
mkfs: mkfs.c fs.h
	gcc -Werror -ggdb -Wall -o mkfs mkfs.c

fs.img: mkfs fs/README $(addprefix fs/,$(UPROGS))
	./mkfs fs.img fs

# make a printout
FILES = $(shell grep -v '^\#' runoff.list)
PRINT = runoff.list runoff.spec $(FILES)

xv6.pdf xv6.ps: $(PRINT)
	./runoff
	ls -l xv6.pdf

.gdbinit: dot-gdbinit
	sed "s/localhost:1234/localhost:$(GDBPORT)/" < $^ > $@

.bochsrc: dot-bochsrc
	ln -s dot-bochsrc .bochsrc

