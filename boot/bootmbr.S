
.globl start
start:
.code16                                             # 16位实模式代码：将BootLoader读进内存并跳转
    cli                                             # 关闭中断响应
    cld                                             # 将串传送指令设置为地址增加的方向

    xorw %ax, %ax                                   # ax => 0
    movw %ax, %ds                                   # -> Data Segment
    movw %ax, %es                                   # -> Extra Segment
    movw %ax, %ss                                   # -> Stack Segment

    movb $2, %ah                                    # 使用BIOS 13号中断以CHS的形式读写硬盘，2表示读取功能
    movb $7, %al                                    # 读取的扇区数
    movb $0, %ch                                    # 柱面号(C)
    movb $0, %dh                                    # 磁头号(H)
    movb $2, %cl                                    # 扇区号(S)，这里的扇区号以1开始计数
    movb $0x80, %dl                                 # 软盘从0x00开始，硬盘从0x80开始
    movw $0x6000, %bx                               # 写到0x6000
    int $0x13
    jmpw *%bx                                       # 跳转至BootLoader
