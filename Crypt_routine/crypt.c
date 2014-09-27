/* polarssl headers */
#include "aes.h"
#include "entropy.h" /* for generating AES key */
#include "ctr_drbg.h" /* for generating AES key */
/* polarssl headers */
#include "polarssl_kernel_support.h"
#define SUCCESS 0

static 				  aes_context *aes_ctx;
static 				  ctr_drbg_context ctr_drbg;
static 				  entropy_context entropy;
static unsigned char  key[32]; // hold 256-bits key
static char 		 *pers = "wm82nZNB8FAfkqXMVD7G"; // our random string :)
static unsigned char  iv[16]="wm82nZNB8FAfkqXM"; //initialization vector
static unsigned char  *input;
static unsigned char  *output;
static size_t 		  input_len=40;
static size_t		  output_len=0;
static int 			  ret;

static int 			  AES_genkey(void);
static int 			  AES_enc(void);

static int __init 	  finit(void)
{
	int error = -EINVAL;
	error = klog_init(KL_INF_L);

	if (error) {
		printk(KERN_ERR "klog_init failed with err=%d", error);
		goto out;
	}

	if(AES_genkey())
		printk(KERN_INFO "AES_genkey() failed\n");
	else
		printk(KERN_INFO "AES_genkey() success!\n");
		
	if(AES_enc())
		printk(KERN_INFO "AES_encrypt() failed\n");
	else
		printk(KERN_INFO "AES_encrypt success!\n");

	out:
		klog_release();
		
	return SUCCESS;
}

static int AES_enc(void)
{
	int ret;
	int i;
	
	input = (unsigned char*)kmalloc(128,GFP_KERNEL);
	output = (unsigned char*)kmalloc(128,GFP_KERNEL);
	
	for(i=0;i<128;i++) {
		output[i]=0;
		input[i]=0;
	}
	
	//memset(input,0,128);
	//memset(output,0.128);
	
	*input ="Cottage out enabledwCottage out enablabc";

	aes_crypt_cbc(aes_ctx,AES_ENCRYPT,24,iv,input,output);
	
	for(i=0;i<5;i++);
	printk(KERN_WARNING "output =%c",output[i]);
	
	return SUCCESS;
}

static int AES_genkey(void)
{
	entropy_init(&entropy);

	if((ret=ctr_drbg_init(&ctr_drbg,entropy_func,&entropy,(unsigned char*) pers,strlen(pers)))!=0) {
		printk(KERN_INFO "ctr_drbg_init() failed!\n");
		return 1;
	}
	else {
		printk(KERN_INFO "ctr_drbg_init() success!\n");
	}
	
	if ((ret=ctr_drbg_random(&ctr_drbg,key,32))!=0) { /* key - is newly generated 256-bit AES key */
		printk(KERN_INFO "ctr_drbg_random() failed\n");
		return 1;
	}
	else {
		printk(KERN_INFO "ctr_drbg_random succes!\n");
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
MODULE_DESCRIPTION("module that try to encrypt data with 256-bit AES key");
