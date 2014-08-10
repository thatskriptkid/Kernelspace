#include <linux/module.h>
#include <linux/kernel.h>	// printk()
#include <linux/fs.h>       // register_blkdev()
#include <linux/genhd.h>    // gendisk structure
#include <linux/blkdev.h>

#define SUCCESS 0

static struct     gendisk *device;               //kernel's representation of individual device 
static int    	  major_number;
static char   	  *device_name="my_block_device";
static struct 	  request_queue *queue;
static spinlock_t lock; 

static int dev_open(struct inode *inode,struct file *filp);
static int dev_release(struct inode *inode,struct file *filp);

static struct block_device_operations file_op = 
{
	.owner=THIS_MODULE,
	.open=dev_open,
	.release=dev_release,
};

static int block_dev_init(void)
{
	major_number=register_blkdev(0,device_name);
	
	if (major_number<=0) {
		printk(KERN_WARNING "register_blkdev() failed\n");
		return 1;
	}
	
	device=alloc_disk(1);
	
	if(device==NULL) {
		printk(KERN_WARNING "gendisk_create() failed\n"); 
		goto unregister;
	}
	
	queue=blk_init_queue(my_request,
	return SUCCESS;
	
	unregister:
		unregister_blkdev(major_number,device_name);
		
	return 1;	
	
}

static int dev_open(struct inode *inode,struct file *filp)
{
	return SUCCESS;
}

static int dev_release(struct inode *inode,struct file *filp)
{
	return SUCCESS;
}

static int __init dev_init(void)
{
	block_dev_init();
	return SUCCESS;
}

static void __exit dev_exit(void)
{
	
	del_gendisk(device);
	unregister_blkdev(major_number,device_name);
	
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("thatskriptkid");
MODULE_DESCRIPTION("create block device and device driver");

