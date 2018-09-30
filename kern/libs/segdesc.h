#ifndef __KERN_LIBS_SEGDESC_H__
#define __KERN_LIBS_SEGDESC_H__

#include <defs.h>

struct pseudodesc {
    uint16_t pd_lim;    // Limit
    ulongptr_t pd_base; // Base address
} __packed;

// 系统描述符的类型
#define SYS_DESC_LDT 0x2            // 64-bit LDT
#define SYS_DESC_TSS_AVAILABLE 0x9  // Available 64-bit TSS
#define SYS_DESC_TSS_BUSY 0xB       // Busy 64-bit TSS
#define SYS_DESC_CALL_GATE 0xC      // 64-bit Call Gate
#define SYS_DESC_INTERRUPT_GATE 0xE // 64-bit Interrupt Gate
#define SYS_DESC_TRAP_GATE 0xF      // 64-bit Trap Gate

struct gatedesc {
    uint32_t gd_off_15_0 : 16;  // low 16 bits of offset in segment
    uint32_t gd_sel : 16;       // segment selector
    uint32_t gd_ist : 3;        // IST in AMD64
    uint32_t gd_rsv1 : 5;       // reserved
    uint32_t gd_type : 4;       // type
    uint32_t gd_s : 1;          // must be 0 (system)
    uint32_t gd_dpl : 2;        // descriptor(meaning new) privilege level
    uint32_t gd_p : 1;          // Present
    uint32_t gd_off_31_16 : 16; // high bits of offset in segment
    uint32_t gd_off_32_63 : 32; // high 32 bits of offset in segment
    uint32_t gd_rsv2 : 32;      // reserved (for Call Gate, bits 8-12 must be zero)
};

//system descriptor分为system-segment descriptor和gate descriptor
#define SET_GATE_DESC(desc, off, sel, ist, type, dpl) \
    {                                                 \
        (desc).gd_off_15_0 = (off)&0xffff;            \
        (desc).gd_sel = (sel);                        \
        (desc).gd_ist = (ist);                        \
        (desc).gd_rsv1 = 0;                           \
        (desc).gd_type = (type);                      \
        (desc).gd_s = 0;                              \
        (desc).gd_dpl = (dpl);                        \
        (desc).gd_p = 1;                              \
        (desc).gd_off_31_16 = ((off) >> 16) & 0xffff; \
        (desc).gd_off_32_63 = (off) >> 32;            \
        (desc).gd_rsv2 = 0;                           \
    }

struct syssegdesc {
    uint32_t ssd_lim_15_0 : 16;   // low bits of segment limit
    uint32_t ssd_base_15_0 : 16;  // low bits of segment base address
    uint32_t ssd_base_23_16 : 8;  // middle bits of segment base address
    uint32_t ssd_type : 4;        // segment type
    uint32_t ssd_s : 1;           // must be 0 (system)
    uint32_t ssd_dpl : 2;         // descriptor Privilege Level
    uint32_t ssd_p : 1;           // present
    uint32_t ssd_lim_19_16 : 4;   // high bits of segment limit
    uint32_t ssd_avl : 1;         // unused (available for software use)
    uint32_t ssd_rsv1 : 2;        // reserved
    uint32_t ssd_g : 1;           // granularity: limit scaled by 4K when set
    uint32_t ssd_base_31_24 : 8;  // high bits of segment base address
    uint32_t ssd_base_63_32 : 32; // high 32 bits of segment base address
    uint32_t ssd_rsv2 : 32;       // reserved (bits 8-12 must be zero)
};
/*
//TODO:
#define SET_SYS_SEG_DESC(desc, limit, base, type, dpl) \
    {                                                  \
        (desc).gd_off_15_0 = (off)&0xffff;             \
    }
//TODO:
#define SET_SEG_DESC(gate, type, sel, off, dpl) \
    {                                           \
        (gate).gd_off_15_0 = (off)&0xffff;      \
    }
*/
#endif /* !__KERN_LIBS_SEGDESC_H__ */
