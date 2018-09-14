#ifndef __BOOT_LOADER_H__
#define __BOOT_LOADER_H__

/* Assembler macros to create x86 segments */

/* Normal segment */
#define SEG_EMPTY .quad 0;

//由于小端字节序的原因，不可以写成一个long或quad（除非反过来写）
#define SEG_CODE \
    .long 0;     \
    .byte 0, 0b10011000, 0b00100000, 0;

#define SEG_DATA \
    .long 0;     \
    .byte 0, 0b10010010, 0, 0;

#endif /* !__BOOT_LOADER_H__ */
