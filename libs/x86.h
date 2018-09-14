#ifndef __LIBS_X86_H__
#define __LIBS_X86_H__

#include <defs.h>

/* Pseudo-descriptors used for LGDT, LLDT(not used) and LIDT instructions. */
struct pseudodesc {
    uint16_t pd_lim;  // Limit
    uint64_t pd_base; // Base address
} __attribute__((packed));

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

// 对指令的封装

static __always_inline uint8_t
inb(uint16_t port) {
    uint8_t data;
    asm volatile("inb %1, %0"
                 : "=a"(data)
                 : "d"(port));
    return data;
}

static __always_inline void
insl(uint32_t port, void *addr, int cnt) {
    asm volatile(
        "cld;"
        "repne; insl;"
        : "=D"(addr), "=c"(cnt)
        : "d"(port), "0"(addr), "1"(cnt)
        : "memory", "cc");
}

static __always_inline void
outb(uint16_t port, uint8_t data) {
    asm volatile("outb %0, %1" ::"a"(data), "d"(port));
}

static __always_inline void
outw(uint16_t port, uint16_t data) {
    asm volatile("outw %0, %1" ::"a"(data), "d"(port));
}

static __always_inline void
lidt(struct pseudodesc *pd) {
    asm volatile("lidt (%0)" ::"r"(pd));
}

static __always_inline void
sti(void) {
    asm volatile("sti");
}

static __always_inline void
cli(void) {
    asm volatile("cli");
}

static __always_inline void
ltr(uint16_t sel) {
    asm volatile("ltr %0" ::"r"(sel));
}

#endif /* !__LIBS_X86_H__ */
