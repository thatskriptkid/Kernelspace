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
#include <asm/segment.h>
#include <linux/buffer_head.h>

#define SUCCESS 0 
#define RNG_NAME "/dev/urandom"

struct file  *filp;
ssize_t      bytes_read;
char __user  *buff;
ssize_t      count;
loff_t       pos;
          
void file_close(struct file* file);

static int __init finit(void)
{
	mm_segment_t old_fs = get_fs();
	
	set_fs(get_ds());
	/*
	filp = filp_open(RNG_NAME,O_RDONLY,0); // in fs/open.c 
	
	if(IS_ERR(filp))
		printk(KERN_WARNING "filp_open() failed!\n");
	else
		printk(KERN_WARNING "filp_open() success!\n");
		
	bytes_read = vfs_read(filp,buff,1,&filp->f_pos); //bytes_read = -14
	
	if (bytes_read<0) 
		printk(KERN_WARNING "vfs_read failed\n");
	else
		printk(KERN_WARNING "bytes_read = %d \n",((int)bytes_read));
	*/
	
	filp = filp_open("/dev/urandom",O_RDONLY,0); 
	
	if(IS_ERR(filp))
		printk(KERN_WARNING "filp_open() failed!\n");
	else
		printk(KERN_WARNING "filp_open() success!\n");
		
	pos=0;
	
	buff =(char*) kmalloc(10,GFP_KERNEL);
	
	bytes_read = vfs_read(filp, buff, 10, &pos); 
	
	if (bytes_read<0) 
		printk(KERN_WARNING "vfs_read failed\n");
	else
		printk(KERN_WARNING "bytes_read = %d \n",((int)bytes_read));
			
	filp_close(filp,NULL);
	
	set_fs(old_fs);
	
	return 0;
}

void file_close(struct file* file) 
{
    filp_close(file, NULL);
}

static void __exit fexit(void)
{
	
}

module_init(finit);
module_exit(fexit);

//traditionally in the end
MODULE_LICENSE("GPL");
MODULE_AUTHOR("thatskriptkid");
MODULE_DESCRIPTION("IO in kernel space!!! :-)");
