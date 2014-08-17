#include <linux/kernel.h>  // printk()
#include <linux/module.h>  // THIS_MODULE
#include <linux/init.h>    // __init(),__exit()
#include <linux/slab.h>    // kmalloc()
#include <linux/string.h>

/* polarssl headers */

#include <aes.h>
#include <entropy.h>  // for generating AES key
#include <ctr_drbg.h> // for generating AES key

/* polarssl headers */

#define SUCCESS 0

static aes_context      *aes_ctx;
static ctr_drbg_context ctr_drbg;
static entropy_context  entropy;
static unsigned char    key[32]; // hold 256-bits key
static char             *pers = "wm82nZNB8FAfkqXMVD7G" // our random string :)
static int              ret;
static unsigned char 	iv[16]="wm82nZNB8FAfkqXM"; //initialization vector
static unsigned char 	input[128];
static unsigned char	output[128];
static size_t 			input_len=40;
static size_t			output_len=0;

static int AES_genkey(void);
static int AES_enc(void);

static int __init finit(void)
{
	if(AES_genkey()) 
		printk(KERN_WARNING "AES_genkey() failed\n");
	
	if(AES_enc()) 
		printk(KERN_WARNING "AES_encrypt() failed\n");
	else 
		printk(KERN_INFO "%s",output);
	return SUCCESS;
}

static int AES_enc(void)
{
	memset(input,0,128);
	input="Cottage out enabledwCottage out enabledw";/40 bytes string
	
	aes_setkey(&aes_ctx,key,256);
	
	aes_crypt_cbc(&aes_ctx,AES_ENCRYPT,24,iv,input,output);
	return SUCCESS;
}

static int AES_genkey(void)
{
	entropy_init(&entropy);
	
	if((ret=ctr_drbg_init(&ctr_drbg,entropy_func,&entropy,(unsigned char*) pers,strlen(pers)))!=0) {
		printk(KERN_WARNING "ctr_drbg_init() failed!\n");
		return 1;
	}
	else printk(KERN_WARNING "succes!\n");
	
	if ((ret=ctr_drbg_random(&ctr_drbg,key,32))!=0) {
		printk(KERN_WARNING "ctr_drbg_random() failed\n");
		return 1;
	}
	else printk(KERN_WARNING "succes!\n");
	
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
