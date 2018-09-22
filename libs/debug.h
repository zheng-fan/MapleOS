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

#endif /* !__LIBS_DEBUG_H__ */
