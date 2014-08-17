#include <linux/kernel.h>  // printk()
#include <linux/module.h>  // THIS_MODULE
#include <linux/init.h>    // __init(),__exit()
#include <linux/slab.h>    // kmalloc()
#include <aes.h>

#define SUCCESS 0


static void __init finit(void)
{
	return 0;
}

static void __exit fexit(void)
{
	
}

module_init(finit);
module_exit(fexit);

//traditionally in the end
MODULE_LICENSE("GPL");
MODULE_AUTHOR("thatskriptkid");
MODULE_DESCRIPTION("module that creates char device");
