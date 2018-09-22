#include <defs.h>
#include <elf.h>
#include <asm.h>

uint32_t SECTSIZE = 512;
struct elfhdr *ELFHDR = ((struct elfhdr *)0x10000); //ELF HEADER的临时存放地址

/* waitdisk - wait for disk ready */
static void waitdisk(void) {
    while ((inb(0x1F7) & 0xC0) != 0x40)
        /* do nothing */;
}

/* readsect - read a single sector at @secno into @dst */
static void readsect(void *dst, uint32_t secno) {
    // wait for disk to be ready
    waitdisk();

    outb(0x1F2, 1);                            // count = 1
    outb(0x1F3, secno & 0xFF);                 //0-7位
    outb(0x1F4, (secno >> 8) & 0xFF);          //8-15位
    outb(0x1F5, (secno >> 16) & 0xFF);         //16-23位
    outb(0x1F6, ((secno >> 24) & 0xF) | 0xE0); //0-3是24-27位，第4位是=0为master，=1为slave，5和7必须为1，第6位=1为LBA，=2为CHS
    outb(0x1F7, 0x20);                         // cmd 0x20 - read sectors

    // wait for disk to be ready
    waitdisk();

    // read a sector
    insl(0x1F0, dst, SECTSIZE / 4);
}

/* *
 * readseg - read @count bytes at @offset from kernel into virtual address @va,
 * might copy more than asked.
 * */
static void readseg(ulongptr_t va, uint32_t count, uint32_t offset) {
    ulongptr_t end_va = va + count;

    // round down to sector boundary
    va -= offset % SECTSIZE;

    // translate from bytes to sectors
    // ELF HEADER从第8个扇区开始（这里的扇区号以0开始计数）
    uint32_t secno = (offset / SECTSIZE) + 8;

    // If this is too slow, we could read lots of sectors at a time.
    // We'd write more to memory than asked, but it doesn't matter --
    // we load in increasing order.
    for (; va < end_va; va += SECTSIZE, secno++) {
        readsect((void *)va, secno);
    }
}

/* bootmain - the entry of bootloader */
void bootmain(void) {
    // 读取kernel的前4KB进内存，保证读取完ELF HEADER即可
    readseg((ulongptr_t)ELFHDR, SECTSIZE * 8, 0);

    // is this a valid ELF?
    if (ELFHDR->e_magic != ELF_MAGIC) {
        goto bad;
    }

    struct proghdr *ph, *eph;

    // load each program segment (ignores ph flags)
    ph = (struct proghdr *)((ulongptr_t)ELFHDR + ELFHDR->e_phoff);
    eph = ph + ELFHDR->e_phnum;
    for (; ph < eph; ph++) {
        readseg(ph->p_va, ph->p_memsz, ph->p_offset);
    }

    // 跳转至内核
    // call the entry point from the ELF header
    // note: does not return
    ((void (*)(void))(ELFHDR->e_entry))();

bad:
    outw(0x8A00, 0x8A00);
    outw(0x8A00, 0x8E00);

    while (1)
        /* do nothing */;
}
