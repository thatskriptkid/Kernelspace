/*
* Error recovery is sometimes best handled with the goto statement. (c) Linux Device Driver, 3rd edtiion,Chapter 2.
* Try to code according to https://www.kernel.org/doc/Documentation/CodingStyle
*/

#include <linux/kernel.h>  // printk()
#include <linux/module.h>  // THIS_MODULE
#include <linux/init.h>    // __init(),__exit()
#include <linux/fs.h>      // alloc_chrdev_region(), struct file_operations
#include <linux/types.h>   // dev_t
#include <linux/cdev.h>    // cdev structure
#include <linux/device.h>  // class_create()
#include <linux/err.h>     // error codes 
#include <asm/uaccess.h>   // copy_to_user(),copy_from_user()
#include <linux/slab.h>    // kmalloc()

#define SUCCESS 0

static struct device *device;//the basic device structure
static char   *DEV_NAME="my_char_device"; //name of our device
static dev_t  DEV_NUM; // device MAJOR and minor numbers
static struct cdev my_cdev; //need for inode structure, represent a character device within the kernel
static struct class *device_class;
static char   *kbuff; //for write method
static size_t kbuff_size=10; //size of kbuff
	
static int     cdev_create(void);
static int     chrdev_init(void);// create char device and add to the system
static void    chrdev_release(void);//release all resources related to our char device
static int     chrdev_open(struct inode *inode,struct file *filp);
static int     chrdev_close(struct inode *inode,struct file *filp);
static ssize_t chrdev_write(struct file *filp,char __user *buff,size_t count,loff_t *offp);

static struct file_operations fops = // function pointers on actions on our device
{
	.owner=THIS_MODULE, //prevent module unloading
	.open=chrdev_open,
	.write=chrdev_write,
	.release=chrdev_close
};

static int __init finit(void) //main() analogue
{
	if(chrdev_init())
		printk(KERN_WARNING "chrdev_init() failed\n");
	else
		printk(KERN_INFO "chrdev_init() successful\n");
	return SUCCESS;
}

static int chrdev_init(void)
{
	device_class=class_create(THIS_MODULE,DEV_NAME);

	if (IS_ERR(device_class)) {
		printk(KERN_WARNING "class_create() failed\n");
		return 1;
	}

	if(alloc_chrdev_region(&DEV_NUM,0,1,DEV_NAME)!=SUCCESS) {//allocate char device number dynamically
		printk(KERN_WARNING "alloc_chrdev_region() failed\n");
		goto class_destroy;
	}

	if(cdev_create()) {
		printk(KERN_WARNING "cdev_create() failed\n");
		goto unreg;
	}

	device=device_create(device_class,NULL,DEV_NUM,NULL,DEV_NAME);//creates a device and registers it with sysfs

	if (IS_ERR(device)) {
		printk(KERN_WARNING "device_create() failed\n");
		goto cdev_del;
	}

	return SUCCESS; //chrdev_init() success

	unreg:
		unregister_chrdev_region(DEV_NUM,1);
	cdev_del:
		cdev_del(&my_cdev);
	class_destroy:
		class_destroy(device_class);
		
	return 1;

}

static int cdev_create(void)
{
	cdev_init(&my_cdev,&fops);

	my_cdev.ops=&fops;
	my_cdev.owner=THIS_MODULE;

	if((cdev_add(&my_cdev,DEV_NUM,1))<0) {
		printk(KERN_WARNING "cdev_add() failed\n");
		return 1;
	}

	return SUCCESS;

}

static int chrdev_open(struct inode *inode,struct file *filp)
{
	printk(KERN_WARNING "device_open() succesful\n");	
	
	inode->i_cdev=&my_cdev;//add cdev struct to our inode
	filp->private_data=&my_cdev;
	filp->f_op=&fops;
	
	return SUCCESS;
}

static ssize_t chrdev_write(struct file *filp,char __user *buff,size_t count,loff_t *offp)
{
	int huint;
	char *tmp;

	tmp=buff+count-1;
	kbuff=kmalloc(1,GFP_KERNEL);
	//memset(kbuff,0,10);// for security reasons
	huint=copy_from_user(kbuff,tmp,1);
	
	if (huint==0)
	printk(KERN_WARNING "write() success!\n");
	else 
	printk(KERN_WARNING "write() failed %d\n",huint);
	
	printk(KERN_WARNING "write() failed %c\n",tmp[0]);
	return 1;
}

static int chrdev_close(struct inode *inode,struct file *filp)
{
	/* There should be code for shutdown hardware device. 
	 * Because we don't have physical device we do nothing here
	*/

	return SUCCESS;
}

static void chrdev_release(void)
{
	kfree(kbuff);
	device_destroy(device_class,DEV_NUM);	//remove a device that was created with device_create
	cdev_del(&my_cdev);
	unregister_chrdev_region(DEV_NUM,1); //free device number
	class_destroy(device_class); //destroy a struct class structure
}

static void __exit fexit(void)
{
	chrdev_release();
}

module_init(finit);
module_exit(fexit);

//traditionally in the end
MODULE_LICENSE("GPL");
MODULE_AUTHOR("thatskriptkid");
MODULE_DESCRIPTION("module that creates char device");
