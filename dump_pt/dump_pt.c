#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "../driver/kdrv_rmem.h"


static int is_leaf(uint64_t val) {
    return (val & 0x80);
}

static int is_present(uint64_t val) {
    return (val & 0x7);
}

struct pt_msg_list {
    struct msg m;
    uint64_t dir;
    struct pt_msg_list *next;
};

int dump_pt(int fd, struct pt_msg_list *p) {
    printf("=========PT(0x%lx)==========\n", p->m.addr);
    struct msg m = { 0 };
    m.addr = p->m.addr & (~0xFFF);

    for (uint64_t i = 0; i < 512; i++) {
        ioctl(fd, RMEM_READ, &m);
        if (is_present(m.data)) {
            printf("0x%lx: 0x%lx", m.addr, m.data);
            printf(": Leaf(4K): 0x%lx\n", p->dir + (0x1000 * i));
        }
        m.addr += 0x8;
    }
    printf("=========PT(0x%lx) END==========\n", p->m.addr);

    return 0;
}

int dump_pd(int fd, struct pt_msg_list *p) {
    printf("=========PD(0x%lx)==========\n", p->m.addr);
    struct msg m = { 0 };
    m.addr = p->m.addr & (~0xFFF);

    struct pt_msg_list *head = NULL;
    struct pt_msg_list *tail = NULL;

    for (uint64_t i = 0; i < 512; i++) {
        ioctl(fd, RMEM_READ, &m);
        if (is_present(m.data)) {
            printf("0x%lx: 0x%lx", m.addr, m.data);
            if (!is_leaf(m.data)) {
                printf(": DIR(2M): 0x%lx\n", p->dir + (0x200000 * i));
                struct pt_msg_list *pt = calloc(1, sizeof(struct pt_msg_list));
                pt->m.addr = m.data;
                pt->dir = p->dir + (i * 0x200000);

                if (head == NULL) {
                    head = pt;
                } else {
                    tail->next = pt;
                }
                tail = pt;
            } else {
                printf(": Leaf(2M): 0x%lx\n", p->dir + (0x200000 * i));
            }
        }
        m.addr += 0x8;
    }
    printf("=========PD(0x%lx) END==========\n", p->m.addr);

    struct pt_msg_list *t = head;
    while (t) {
        dump_pt(fd, t);
        t = t->next;
    }

    return 0;
}

int dump_pdpt(int fd, struct pt_msg_list *p) {
    printf("=========PDPT(0x%lx)==========\n", p->m.addr);
    struct msg m = { 0 };
    m.addr = p->m.addr & (~0xFFF);

    struct pt_msg_list *head = NULL;
    struct pt_msg_list *tail = NULL;

    for (uint64_t i = 0; i < 512; i++) {
        ioctl(fd, RMEM_READ, &m);
        if (is_present(m.data)) {
            printf("0x%lx: 0x%lx", m.addr, m.data);
            if (!is_leaf(m.data)) {
                printf(": DIR(1G): 0x%lx\n", 0x40000000UL * i);
                struct pt_msg_list *pd = calloc(1, sizeof(struct pt_msg_list));
                pd->m.addr = m.data;
                pd->dir += p->dir + (i * 0x40000000);

                if (head == NULL) {
                    head = pd;
                } else {
                    tail->next = pd;
                }
                tail = pd;
            } else {
                printf(": Leaf(1G): 0x%lx\n", 0x40000000UL * i);
            }
        }
        m.addr += 0x8;
    }
    printf("=========PDPT(0x%lx) END==========\n", p->m.addr);

    struct pt_msg_list *t = head;
    while (t) {
        dump_pd(fd, t);
        t = t->next;
    }

    return 0;
}

int dump_pml4(int fd, uint64_t pml4) {
    printf("=========PML4(0x%lx)==========\n", pml4);
    struct msg m = { 0 };
    m.addr = pml4 & (~0xFFF);

    struct pt_msg_list *head = NULL;
    struct pt_msg_list *tail = NULL;

    for (uint64_t i = 0; i < 512; i++) {
        ioctl(fd, RMEM_READ, &m);
        if (is_present(m.data)) {
            printf("0x%lx: 0x%lx", m.addr, m.data);
            if (!is_leaf(m.data)) {
                printf("\n");
                struct pt_msg_list *pdpt = calloc(1, sizeof(struct pt_msg_list));
                pdpt->m.addr = m.data;
                pdpt->dir = 0;

                if (head == NULL) {
                    head = pdpt;
                } else {
                    tail->next = pdpt;
                }
                tail = pdpt;
            }
        }
        m.addr += 0x8;
    }
    printf("=========PML4(0x%lx) END==========\n", pml4);

    struct pt_msg_list *p = head;
    while (p) {
        dump_pdpt(fd, p);
        p = p->next;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage:\n");
        printf("    %s [physical addr]\n", argv[0]);
        return -1;
    }

    struct msg m = { 0 };
    m.addr = strtoul(argv[1], NULL, 0);

    int fd = open("/dev/rmem", O_RDWR);
    if (fd < 0) {
        printf("Failed to open /dev/rmem\n");
        return -1;
    }

    dump_pml4(fd, m.addr);

    close(fd);

    return 0;
}
