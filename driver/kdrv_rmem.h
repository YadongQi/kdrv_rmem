#ifndef KDRV_RMEM_H
#define KDRV_RMEM_H

#include <linux/ioctl.h>

#define RMEM_READ _IOW('r','r',uint64_t*)

struct msg {
	uint64_t addr;
	uint64_t data;
};

#endif
