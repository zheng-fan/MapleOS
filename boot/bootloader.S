#include <bootloader.h>

.set PROTECTED_MODE_CSEG,        0x8                # 0x8 = 0b01000, index=1, TI=0, RPL=0 内核的代码段在GDT的信息
.set PROTECTED_MODE_DSEG,        0x10               # 0x8 = 0b10000, index=2, TI=0, RPL=0 内核的数据段在GDT的信息

.globl start
start:
.code16                                             # 16位实模式代码：进入64位长模式

set_A20.1:                                          # 开启A20地址线
    inb $0x64, %al                                  # Wait for not busy(8042 input buffer empty).
    testb $0x2, %al
    jnz set_A20.1

    movb $0xd1, %al                                 # 0xd1 -> port 0x64
    outb %al, $0x64                                 # 0xd1 means: write data to 8042's P2 port

set_A20.2:
    inb $0x64, %al                                  # Wait for not busy(8042 input buffer empty).
    testb $0x2, %al
    jnz set_A20.2

    movb $0xdf, %al                                 # 0xdf -> port 0x60
    outb %al, $0x60                                 # 0xdf = 11011111, means set P2's A20 bit(the 1 bit) to 1

    movl $0x1000, %edi
    movl %edi, %cr3                                 # 设置PML4页表的地址
    xorl %eax, %eax
    movl $4096, %ecx                                # 清空4级页表
    rep stosl
    movl %cr3, %edi                                 # stosl指令会修改edi，所以要改回去

    movl $0x2000 | 3, (%edi)                        # 设置PDP页表的地址，3代表读写权限
    addl $0x1000, %edi
    movl $0x3000 | 3, (%edi)                        # 设置PD页表的地址
    addl $0x1000, %edi
    movl $0x4000 | 3, (%edi)                        # 设置PT页表的地址（最后一级页表）
    addl $0x1000, %edi

    movl $3, %ebx                                   # 3代表读写权限
    movl $512, %ecx
set_mem_entry:                                      # 设置PT页表中的512个entry，从0开始，共计2M的空间
    movl %ebx, (%edi)                               # 此时edi=0x4000
    addl $0x1000, %ebx
    addl $8, %edi                                   # 前面已经清空过了，所以高32位无需再处理
    loop set_mem_entry

    movl %cr4, %eax
    orl $1<<5, %eax
    movl %eax, %cr4                                 # CR4.PAE=1 开启PAE分页

    movl $0xC0000080, %ecx
    rdmsr
    orl $1<<8, %eax                                 # EFER.LME=1 开启长模式
    wrmsr

    movl %cr0, %eax
    orl $1<<31|1, %eax                              # CR0.PG=1 CR0.PE=1
    movl %eax, %cr0                                 # 开启分页，开启保护模式

    lgdt gdt_info                                   # 加载GDT

    ljmp $PROTECTED_MODE_CSEG, $protected_code_seg  # 进入长模式

.code64                                             # 64位长模式代码
protected_code_seg:
    movw $PROTECTED_MODE_DSEG, %ax                  # 设置64位长模式下的段选择子，使用相同的段
    movw %ax, %ds                                   # -> DS: Data Segment
    movw %ax, %es                                   # -> ES: Extra Segment
    movw %ax, %fs                                   # -> FS
    movw %ax, %gs                                   # -> GS
    movw %ax, %ss                                   # -> SS: Stack Segment

    movq $start, %rsp                               # 给BootLoader一个临时的栈，从start(0x6000)开始向低地址延伸
    call bootmain                                   # 进入C程序

spin:
    jmp spin                                        # 正常情况下不会执行到这里

.p2align 3                                          # 对GDT进行8字节对齐，提高性能
gdt:
    SEG_NULL                                        # GDT第一个要为空
    SEG_CODE                                        # BootLoader和kernel的代码段
    SEG_DATA                                        # BootLoader和kernel的数据段

gdt_info:
    .word 0x17                                      # GDT Limit = sizeof(gdt) - 1
    .quad gdt                                       # GDT的起始地址
