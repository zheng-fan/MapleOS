# vector.S中的所有中断都会跳转到此处进行处理
.text
.globl __alltraps
__alltraps:
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %rsp
    pushq %rsi
    pushq %rdi
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11

    # 将trap frame的地址作为参数传递给trap_dispatch函数
    movq %rsp, %rdi

    call trap_dispatch

    popq %rsp

    # 恢复被保存的寄存器
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdi
    popq %rsi
    popq %rsp
    popq %rdx
    popq %rcx
    popq %rax

    # 跳过trap number和error code
    addq $0x10

    iret
