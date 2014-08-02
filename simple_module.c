#include <linux/kernel.h> //printk()
#include <linux/module.h> //THIS_MODULE
#include <linux/init.h> // __init,__exit
#include <linux/fs.h> //alloc_chrdev_region(), struct file_operations
#include <linux/types.h> // dev_t
#include <linux/cdev.h> // cdev structure
#include <linux/device.h> //class_create()
#include <linux/err.h> //for error codes (ERR_PTR)

#define SUCCESS 0
#define DEV_RANGE 300 // device number range 

static struct device *device;//the basic device structure 
static char *DEV_NAME="my_char_device"; //name of our device
static dev_t DEV_NUM; // device MAJOR and minor numbers
static struct inode *chr_inode; //represent char device internally, in kernel
static struct cdev *my_cdev; //need for inode structure, represent a character device within the kernel
static struct class *device_class;
static struct file_operations fops = // function pointers on actions on our device
{
	.owner=THIS_MODULE, //prevent module unloading
	
};
static void cdev_create(void);// create cdev structure

static void chrdev_init(void)
{
	device_class=class_create(THIS_MODULE,DEV_NAME);// create a struct class structure 
	
	if(alloc_chrdev_region(&DEV_NUM,0,1,DEV_NAME)!=SUCCESS)
		{
			printk(KERN_WARNING "register_chrdev_region() failed\n"); //allocate char device number dynamically
			goto unreg;
		}
		
	if(!DEV_NUM)
		{
			printk(KERN_WARNING "DEV_NUM was not set\n");
			goto unreg;
		}
	
	cdev_create();
	
	device=device_create(device_class,NULL,DEV_NUM,NULL,DEV_NAME);//creates a device and registers it with sysfs
	
		if (IS_ERR(device)) 
		{
			printk(KERN_WARNING "device_create() failed\n");
			goto cdev_del;
		}
		
		
	unreg:
		unregister_chrdev_region(DEV_NUM,1);//free device number 
		
	cdev_del:
		cdev_del(my_cdev);
	
}

static void cdev_create(void)
{
	if((my_cdev=cdev_alloc())==NULL)
		printk(KERN_WARNING "cdev_alloc() failed\n");
	
	
	cdev_init(my_cdev,&fops);
	
	my_cdev->ops=&fops;
	my_cdev->owner=THIS_MODULE;
	
	if((cdev_add(my_cdev,DEV_NUM,1))<0)
		{
			printk(KERN_WARNING "cdev_add() failed\n");
			cdev_del(my_cdev);
		}
	
}



static int __init finit(void)
{
	chrdev_init();

	return 0;
}

static void __exit fexit(void)
{
	cdev_del(my_cdev);
	unregister_chrdev_region(DEV_NUM,1);//free device number 
	class_destroy(device_class); //destroys a struct class structure 
	
}

module_init(finit);
module_exit(fexit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("thatskriptkid");
MODULE_DESCRIPTION("module that create char device");
