#ifndef __LIBS_DEBUG_H__
#define __LIBS_DEBUG_H__

#define read_reg16(reg) ({             \
    uint16_t reg;                      \
    asm volatile("movw %%" #reg ", %0" \
                 : "=r"(reg));         \
    reg;                               \
})

#define read_reg32(reg) ({             \
    uint32_t reg;                      \
    asm volatile("movl %%" #reg ", %0" \
                 : "=r"(reg));         \
    reg;                               \
})

#define read_reg64(reg) ({             \
    uint64_t reg;                      \
    asm volatile("movq %%" #reg ", %0" \
                 : "=r"(reg));         \
    reg;                               \
})

static const char *rflags_name[] = {
    "(CF) Carry Flag",
    "(Reserved 1) Read as One",
    "(PF) Parity Flag",
    "(Reserved 3) Read as Zero",
    "(AF) Auxiliary Flag",
    "(Reserved 5) Read as Zero",
    "(ZF) Zero Flag",
    "(SF) Sign Flag",
    "(TF) Trap Flag",
    "(IF) Interrupt Flag",
    "(DF) Direction Flag",
    "(OF) Overflow Flag",
    "(IOPL:12) I/O Privilege Level",
    "(IOPL:13) I/O Privilege Level",
    "(NT) Nested Task",
    "(Reserved 15) Read as Zero",
    "(RF) Resume Flag",
    "(VM) Virtual-8086 Mode",
    "(AC) Alignment Check",
    "(VIF) Virtual Interrupt Flag",
    "(VIP) Virtual Interrupt Pending",
    "(ID) ID Flag",
    // 63:22 Reserved Reserved, Read as Zero
};

#endif /* !__LIBS_DEBUG_H__ */
