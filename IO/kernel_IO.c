#include <linux/kernel.h>  // printk()
#include <linux/module.h>  // THIS_MODULE
#include <linux/init.h>    // __init(),__exit()
#include <linux/slab.h>    // kmalloc()
#include <linux/string.h>
#include <linux/syscalls.h>
#include <linux/fcntl.h>
#include <asm/uaccess.h>
#include <linux/types.h>    //umode_t
#include <linux/fs.h>

#define SUCCESS 0 

MODULE_LICENSE("GPL");

static int __init finit(void)
{
	struct file *filp=NULL;
	
	filp = filp_open("/dev/urandom",O_RDONLY,0); // in fs/open.c 
	
	
	return 0;
}

static void __exit fexit(void)
{
	
}

module_init(finit);
module_exit(fexit);

//traditionally in the end
MODULE_AUTHOR("thatskriptkid");
MODULE_DESCRIPTION("IO in kernel space!!! :-)");
