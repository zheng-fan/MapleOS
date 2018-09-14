#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    struct stat st;
    if (argc <= 2) {
        fprintf(stderr, "Usage: -bootmbr <input filename> <output filename> or -bootloader <input filename>\n");
        return -1;
    }
    //检查BootLoader是否超过7个扇区的大小
    if (argc == 3 && strcmp(argv[1], "-bootloader") == 0) {
        if (stat(argv[2], &st) != 0) {
            fprintf(stderr, "Error opening file '%s': %s\n", argv[2], strerror(errno));
            return -1;
        }
        printf("'%s' size: %lld bytes\n", argv[2], (long long)st.st_size);
        if (st.st_size > 3584) {
            fprintf(stderr, "%lld >> 3584!!\n", (long long)st.st_size);
            return -1;
        }
        return 0;
    }
    if (!(argc == 4 && strcmp(argv[1], "-bootmbr") == 0)) {
        return -1;
    }
    if (stat(argv[2], &st) != 0) {
        fprintf(stderr, "Error opening file '%s': %s\n", argv[2], strerror(errno));
        return -1;
    }
    printf("'%s' size: %lld bytes\n", argv[2], (long long)st.st_size);
    if (st.st_size > 510) {
        fprintf(stderr, "%lld >> 510!!\n", (long long)st.st_size);
        return -1;
    }
    char buf[512];
    memset(buf, 0, sizeof(buf));
    FILE *ifp = fopen(argv[2], "rb");
    int size = fread(buf, 1, st.st_size, ifp);
    if (size != st.st_size) {
        fprintf(stderr, "read '%s' error, size is %d.\n", argv[2], size);
        return -1;
    }
    fclose(ifp);
    buf[510] = 0x55;
    buf[511] = 0xAA;
    FILE *ofp = fopen(argv[3], "wb+");
    size = fwrite(buf, 1, 512, ofp);
    if (size != 512) {
        fprintf(stderr, "write '%s' error, size is %d.\n", argv[3], size);
        return -1;
    }
    fclose(ofp);
    printf("build 512 bytes boot sector: '%s' success!\n", argv[3]);
    return 0;
}
