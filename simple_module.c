#include <linux/kernel.h> //printk()
#include <linux/module.h> //THIS_MODULE
#include <linux/init.h> // __init,__exit
#include <linux/fs.h> //alloc_chrdev_region(), struct file_operations
#include <linux/types.h> // dev_t
#include <linux/cdev.h> // cdev structure

#define SUCCESS 0
#define DEV_RANGE 300 // device number range 
#define DEV_NAME "my_char_device" //name of our device

static dev_t DEV_NUM; // device MAJOR and minor numbers
static struct inode *inode; //represent char device internally, in kernel
static struct cdev *my_cdev; //need for inode structure
static struct file_operations fops = // function pointers on actions on our device
{
	.owner=THIS_MODULE, //prevent module unloading
	
};
struct file *filep;//represent file in kernel

static void cdev_create(void)
{
	my_cdev=cdev_alloc();
	my_cdev->ops=&fops;
	my_cdev->owner=THIS_MODULE;
	
	cdev_init(my_cdev,&fops);
	
	if((cdev_add(my_cdev,DEV_NUM,1))<0)
		{
			printk(KERN_WARNING "cdev_add() failed\n");
			cdev_del(my_cdev);
		}
	else inode->i_cdev=my_cdev; //add cdev structure to our inode
	
}

static void chrdev_init(void)
{
	if(alloc_chrdev_region(&DEV_NUM,0,DEV_RANGE,DEV_NAME)!=SUCCESS)
		{
			printk(KERN_WARNING "register_chrdev_region() failed\n"); //allocate char device number dynamically
			goto unreg;
		}
	else printk(KERN_WARNING "register_chrdev_region() successful\n");	
	
	if(!DEV_NUM)
		{
			printk(KERN_WARNING "DEV_NUM was not set\n");
			goto unreg;
		}
	else printk(KERN_WARNING "DEV_NUM is set\n");	
	
	/*
	filep->f_mode=FMODE_READ|FMODE_WRITE;
	filep->f_op=&fops;
	*/
	cdev_create();
	
		
	unreg:
		unregister_chrdev_region(DEV_NUM,300);//free device number 
	
}

static int __init finit(void)
{
	chrdev_init();
	return 0;
}

static void __exit fexit(void)
{
	
	
}

module_init(finit);
module_exit(fexit);

