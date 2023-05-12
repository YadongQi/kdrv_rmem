#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "../driver/kdrv_rmem.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage:\n");
        printf("    %s [physical addr]\n", argv[0]);
        return -1;
    }

    struct msg m = { 0 };
    m.addr = strtoul(argv[1], NULL, 0);
    printf("phyaddr=0x%lx\n", m.addr);

    int fd = open("/dev/rmem", O_RDWR);
    if (fd < 0) {
        printf("Failed to open /dev/rmem\n");
        return -1;
    }
    ioctl(fd, RMEM_READ, &m);
    printf("data: 0x%lx\n", m.data);

    close(fd);

    return 0;
}
