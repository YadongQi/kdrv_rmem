#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/err.h>
#include <asm/io.h>

#include "kdrv_rmem.h"

MODULE_DESCRIPTION("MEM module");
MODULE_AUTHOR("Qi, Yadong");
MODULE_LICENSE("Dual BSD/GPL");


static int rmem_open(struct inode* inode, struct file * file) {
	pr_info("rmem opened!\n");
	return 0;
}

static long rmem_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
	struct msg m;
	uint64_t *va = NULL;
	pr_info("rmem ioctl!\n");
	switch (cmd) {
	case RMEM_READ:
		if (copy_from_user(&m, (struct msg *)arg, sizeof(m))) {
			pr_err("Failed to read message!\n");
			break;
		}
		va = phys_to_virt(m.addr);
		pr_info("phys=0x%llx, virt=%p\n", m.addr, va);
		m.data = *va;
		if (copy_to_user((struct msg *)arg, &m, sizeof(m))) {
			pr_err("Failed to copy back!\n");
			break;
		}
		break;
	default:
		pr_err("Unknown IOCTL cmd!\n");
		break;
	}
	return 0;
}

static const struct file_operations rmem_fops = {
	.open		= rmem_open,
	.unlocked_ioctl = rmem_ioctl,
};

dev_t dev = 0;
static struct class *rmem_class;
static struct cdev rmem_cdev;

static int __init chr_dev_init(void)
{
        if((alloc_chrdev_region(&dev, 0, 1, "rmem")) <0){
                pr_err("Cannot allocate major number\n");
                return -1;
        }
        pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
        cdev_init(&rmem_cdev, &rmem_fops);
 
        if((cdev_add(&rmem_cdev, dev, 1)) < 0){
            pr_err("Cannot add the device to the system\n");
            goto r_class;
        }
 
        if(IS_ERR(rmem_class = class_create(THIS_MODULE, "rmem_class"))){
            pr_err("Cannot create the struct class\n");
            goto r_class;
        }
 
        /*Creating device*/
        if(IS_ERR(device_create(rmem_class, NULL, dev, NULL, "rmem"))){
            pr_err("Cannot create the Device 1\n");
            goto r_device;
        }
        pr_info("KMEM Device Insert...Done!!!\n");
        return 0;
 
r_device:
        class_destroy(rmem_class);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;

}

static void __exit chr_dev_clean(void) {
        device_destroy(rmem_class, dev);
        class_destroy(rmem_class);
        cdev_del(&rmem_cdev);
        unregister_chrdev_region(dev, 1);
}

static int mod_init(void)
{
        pr_err("%s: INIT\n", __func__);
	chr_dev_init();
        return 0;
}

static void mod_exit(void)
{
        pr_err("%s: Exit\n", __func__);
	chr_dev_clean();
}

module_init(mod_init);
module_exit(mod_exit);
