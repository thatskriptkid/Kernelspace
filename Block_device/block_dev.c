#include <linux/module.h>
#include <linux/kernel.h>	// printk()
#include <linux/fs.h>       // register_blkdev()
#include <linux/genhd.h>    // gendisk structure
#include <linux/blkdev.h>   // block_device_operations
#include <linux/spinlock.h> // spin_lock_init()

#define SUCCESS 0
#define KERNEL_SECTOR_SIZE 512

static struct gendisk        *gd;                        //kernel's representation of individual device 
static int    	  			 major_number;
static char   	 			 *device_name="my_block_device";
static struct request_queue  *queue;
static spinlock_t 			 lock; 
static int                   nsectors = 1024;                 /* How big the drive is */
static int					 hardsect_size = 512;

static struct my_block_device {
	unsigned long size;
	spinlock_t lock;
	u8 *data;
	struct gendisk *gd;
} device;


static int  gendisk_create(void);
static void my_request(struct request_queue *queue); /*This function is called whenever the kernel believes it is time 
												      *for your driver to process
												      *some reads, writes, or other operations on the device.
												      */

static struct block_device_operations file_op = 
{
	.owner=THIS_MODULE,
	.open=NULL,
	.release=NULL,
	
};

static int block_dev_init(void)
{
	major_number=register_blkdev(0,device_name);
	
	if (major_number<=0) {
		printk(KERN_WARNING "register_blkdev() failed\n");
		return 1;
	}
	else printk(KERN_WARNING "register_blkdev() success\n");
	
	spin_lock_init(&lock);
	
	queue=blk_init_queue(my_request,&lock);  //prepare a request queue for use with a block device 
	
	if(queue==NULL) {
		printk(KERN_WARNING "blk_init_queue() failed\n");
		goto unregister;
	}
	else printk(KERN_WARNING "blk_init_queue() success\n"); 
	
	if(gendisk_create()) {
		printk(KERN_WARNING "gendisk_create() failed\n"); 
		goto free_queue;
	}
	else printk(KERN_WARNING "gendisk_create() success\n"); 
	
	
	
	
	return SUCCESS;
	
	unregister:
		unregister_blkdev(major_number,device_name);
	free_gendisk:
		del_gendisk(device);
	free_queue:
		blk_cleanup_queue(queue);
		
	return 1;	
	
}

static int gendisk_create(void)
{
	device=alloc_disk(1); //create gendisk structure
	
	if(device==NULL) {
		printk(KERN_WARNING "gendisk_create() failed\n"); 
		return 1;
	}
	
	device->major=major_number;
	device->first_minor=0;
	device->minors=2;// allow us to have 1 partition
	
	strcpy(device->disk_name,"my_block_device0");//name for partition
	
	device->fops=&file_op;
	device->queue=queue;
	
	return SUCCESS;
}

void my_request(struct request_queue *queue)
{
	struct request *req;
	
	while((req=blk_fetch_request(queue))!=NULL) {
		
		 /*tells us whether we are looking at a 
		  *filesystem request — one that moves blocks of data.
		  */
		  
		if (req->cmd_type!=REQ_TYPE_FS) {
			printk(KERN_INFO "Skip non-fs request\n");
			blk_end_request(req,-1,10); 
			continue;
		}
		
		my_block_device_transfer(
		blk_end_request(req,SUCCESS,10);
	}
}
static int __init dev_init(void)
{
	if(block_dev_init())
		printk(KERN_WARNING "block_dev_init() failed\n");
		
	return SUCCESS;
}

static void __exit dev_exit(void)
{
	del_gendisk(device);
	blk_cleanup_queue(queue);
	
	unregister_blkdev(major_number,device_name);
	
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("thatskriptkid");
MODULE_DESCRIPTION("create block device and device driver");

