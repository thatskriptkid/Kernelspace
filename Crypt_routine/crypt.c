#include <linux/kernel.h>  // printk()
#include <linux/module.h>  // THIS_MODULE
#include <linux/init.h>    // __init(),__exit()
#include <linux/slab.h>    // kmalloc()
#include <linux/string.h>

/* polarssl headers */

#include <aes.h>
#include <entropy.h> // for generating AES key
#include <ctr_drbg.h> // for generating AES key

/* polarssl headers */

#define SUCCESS 0

static ctr_drbg_context ctr_drbg;
static entropy_context entropy;
static unsigned char key[32]; // hold 256-bits key
static char *pers = "wm82nZNB8FAfkqXMVD7G" // our random string :)
static int ret;

static int AES_genkey(void);

static void __init finit(void)
{
	if(AES_genkey()) {
		printk(KERN_WARNING "AES_genkey() failed\n");
	return SUCCESS;
}

static int AES_genkey(void)
{
	entropy_init(&entropy);
	
	if((ret=ctr_drbg_init(&ctr_drbg,entropy_func,&entropy,(unsigned char*) pers,strlen(pers)))!=0) {
		printk(KERN_WARNING "ctr_drbg_init() failed!\n");
		return 1;
	}
	
	if ((ret=ctr_drbg_random(&ctr_drbg,key,32))!=0) {
		printk(KERN_WARNING "ctr_drbg_random() failed\n");
		return 1;
	}
	
	return SUCCESS;
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
