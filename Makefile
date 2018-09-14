EMPTY	:=
SPACE	:= $(EMPTY) $(EMPTY)
SLASH	:= /

CC		:= gcc
# 使用-gstabs生成stabs格式的调试信息。使用-ggdb会默认使用DWARF格式的调试信息，等同于-gdwarf
CFLAGS	:= -fno-builtin -Wall -m64 -gstabs -nostdinc -fno-stack-protector

LD			:= ld
LDFLAGS		:= -nostdlib

OBJCOPY 	:= objcopy
OBJDUMP 	:= objdump

COPY		:= cp
MKDIR   	:= mkdir -p
MV			:= mv
RM			:= rm -f
AWK			:= awk
SED			:= sed
SH			:= sh
TR			:= tr
TOUCH		:= touch -c
DD			:= dd

OBJDIR		:= obj
BINDIR		:= bin

QEMU		:= qemu-system-x86_64
MapleOS		:= MapleOS.img
TERMINAL	:= gnome-terminal
PORTFLAGS	:= -parallel null -serial stdio

# -------------------------------------------------------------------
# kernel

$(MapleOS):
	$(MKDIR) $(OBJDIR)/kern/driver/
	$(MKDIR) $(OBJDIR)/kern/init/
	$(MKDIR) $(OBJDIR)/kern/libs/
	$(MKDIR) $(OBJDIR)/boot/
	$(MKDIR) $(OBJDIR)/libs/
	$(MKDIR) $(BINDIR)

	$(CC) $(CFLAGS) -Ilibs/ -Ikern/driver/ -c kern/driver/console.c -o $(OBJDIR)/kern/driver/console.o
	$(CC) $(CFLAGS) -Ilibs/ -Ikern/driver/ -c kern/init/init.c -o $(OBJDIR)/kern/init/init.o
	$(CC) $(CFLAGS) -Ilibs/ -Ikern/driver/ -c kern/libs/stdio.c -o $(OBJDIR)/kern/libs/stdio.o

	$(CC) $(CFLAGS) -Ilibs/ -c libs/printfmt.c -o $(OBJDIR)/libs/printfmt.o
	$(CC) $(CFLAGS) -Ilibs/ -c libs/string.c -o $(OBJDIR)/libs/string.o
	$(LD) $(LDFLAGS) -z max-page-size=0x1000 -T tools/kernel.ld -o $(BINDIR)/kernel $(OBJDIR)/kern/driver/console.o $(OBJDIR)/kern/init/init.o $(OBJDIR)/kern/libs/stdio.o $(OBJDIR)/libs/printfmt.o $(OBJDIR)/libs/string.o

	$(CC) $(CFLAGS) -Ilibs/ -Iboot/ -Os -c boot/bootmbr.S -o $(OBJDIR)/boot/bootmbr.o
	$(CC) $(CFLAGS) -Ilibs/ -Iboot/ -Os -c boot/bootloader.S -o $(OBJDIR)/boot/bootloader.o
	$(CC) $(CFLAGS) -Ilibs/ -Iboot/ -Os -c boot/bootmain.c -o $(OBJDIR)/boot/bootmain.o

	$(CC) -g -Wall -O2 -Itools/ tools/sign.c -o $(BINDIR)/sign

	$(LD) --oformat=elf64-x86-64 $(LDFLAGS) -N -e start -Ttext 0x7C00 $(OBJDIR)/boot/bootmbr.o -o $(OBJDIR)/bootmbr.o
	$(LD) --oformat=elf64-x86-64 $(LDFLAGS) -N -e start -Ttext 0x6000 $(OBJDIR)/boot/bootloader.o $(OBJDIR)/boot/bootmain.o -o $(OBJDIR)/bootloader.o
	$(OBJCOPY) -S -O binary $(OBJDIR)/bootmbr.o $(OBJDIR)/bootmbr.out
	$(OBJCOPY) -S -O binary $(OBJDIR)/bootloader.o $(OBJDIR)/bootloader
	$(BINDIR)/sign -bootmbr $(OBJDIR)/bootmbr.out $(OBJDIR)/bootmbr
	$(BINDIR)/sign -bootloader $(OBJDIR)/bootloader
	$(DD) if=/dev/zero of=$(BINDIR)/$(MapleOS) count=10000
	$(DD) if=$(OBJDIR)/bootmbr of=$(BINDIR)/$(MapleOS) conv=notrunc
	$(DD) if=$(OBJDIR)/bootloader of=$(BINDIR)/$(MapleOS) seek=1 conv=notrunc
	$(DD) if=$(BINDIR)/kernel of=$(BINDIR)/$(MapleOS) seek=8 conv=notrunc

qemu: $(MapleOS)
	$(QEMU) -drive file=$(BINDIR)/$<,index=0,if=ide,media=disk,format=raw $(PORTFLAGS)

debug1: $(MapleOS)
	$(TERMINAL) -- $(QEMU) -S -s -d in_asm -D $(BINDIR)/debug.log -monitor stdio -drive file=$(BINDIR)/$<,index=0,if=ide,media=disk,format=raw
	sleep 1
	$(TERMINAL) -- gdb -q -x tools/bootmbr.gdbinit

debug2: $(MapleOS)
	$(TERMINAL) -- $(QEMU) -S -s -d in_asm -D $(BINDIR)/debug.log -monitor stdio -drive file=$(BINDIR)/$<,index=0,if=ide,media=disk,format=raw
	sleep 1
	$(TERMINAL) -- gdb -q -x tools/bootloader.gdbinit

debug: $(MapleOS)
	$(TERMINAL) -- $(QEMU) -S -s -d in_asm -D $(BINDIR)/debug.log -monitor stdio -drive file=$(BINDIR)/$<,index=0,if=ide,media=disk,format=raw
	sleep 1
	$(TERMINAL) -- gdb -q -x tools/kernel.gdbinit

all: $(MapleOS)

clean:
	$(RM) -r $(OBJDIR) $(BINDIR)
