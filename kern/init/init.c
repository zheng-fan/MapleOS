#include <console.h>
#include <debug.h>
#include <defs.h>
#include <stdio.h>
#include <string.h>
__noreturn void kern_init(void);
void kern_init(void) {
    extern char edata[], end[];
    // 清空未初始化的静态空间（bss段）
    memset(edata, 0, end - edata);

    cons_init(); // init the console
    printf("Hello, World!\n");
    printf("edata=%p, end=%p, end-edata=%p\n", edata, end, end - edata);
    printf("edata=%lld, end=%lld, end-edata=%lld\n", edata, end, end - edata);
    printf("cs=0x%04x, rsp=0x%llx, eax=0x%x\n", (unsigned)read_reg16(cs), read_reg64(rsp), read_reg32(eax));

    while (1)
        /* do nothing */;
}
