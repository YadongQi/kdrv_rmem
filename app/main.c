#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "../driver/kdrv_rmem.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage:\n");
        printf("    %s [physical addr] [offset count]\n", argv[0]);
        return -1;
    }

    struct msg m = { 0 };
    m.addr = strtoul(argv[1], NULL, 0);
    unsigned int count = strtoul(argv[2], NULL, 0);
    if (count > 512) {
        printf("offset count too large!\n");
        return -1;
    }

    int fd = open("/dev/rmem", O_RDWR);
    if (fd < 0) {
        printf("Failed to open /dev/rmem\n");
        return -1;
    }
    for (int i = 0; i < count; i++) {
        ioctl(fd, RMEM_READ, &m);
        printf("0x%lx: 0x%lx\n", m.addr, m.data);
        m.addr += 0x8;
    }

    close(fd);

    return 0;
}
