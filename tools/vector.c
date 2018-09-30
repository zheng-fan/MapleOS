#include <stdio.h>

int main(void) {
    printf(".text\n");
    printf(".globl __alltraps\n");

    int i;
    for (i = 0; i < 256; i++) {
        printf(".globl vector%d\n", i);
        printf("vector%d:\n", i);
        // 对于没有返回值的中断，手动进行占位，以保持栈结构的一致性
        // 9、15、20~28、31是保留的中断
        if ((i <= 7 || i >= 16) && i != 17) {
            printf("  pushq $0\n");
        }
        printf("  pushq $%d\n", i);
        printf("  jmp __alltraps\n");
    }
    printf("\n");
    printf("# vector table\n");
    printf(".data\n");
    printf(".globl __vectors\n");
    // 生成中断服务例程的地址，便于内核读取
    printf("__vectors:\n");
    for (i = 0; i < 256; i++) {
        printf("  .quad vector%d\n", i);
    }
    return 0;
}
