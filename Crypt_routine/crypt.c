/* polarssl headers */
#include "entropy.h" /* for generating AES key */
#include "ctr_drbg.h" /* for generating AES key */
/* polarssl headers */

#include "polarssl_kernel_support.h"

#define SUCCESS 0

static 				  ctr_drbg_context ctr_drbg_ctx;
static 				  entropy_context entropy;
static unsigned char  key[32]; // hold 256-bits key
static const unsigned char 		 *pers = "wm82nZNB8FAfkqXMVD7G"; // our random string :)
static unsigned char  iv[16]="wm82nZNB8FAfkqXM"; //initialization vector
static unsigned char  input[128]="Cottage out enabledwCottage out enablqwe";
static unsigned char  output[128];
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
	
	memset(output,0,128);
	
	if(aes_crypt_cbc(&ctr_drbg_ctx.aes_ctx,AES_ENCRYPT,32,iv,input,output)!=0) {
		printk(KERN_WARNING "aes_crypt_cbc() failed!\n");
		return 1;
	}
	
	return SUCCESS;
}

static int AES_genkey(void)
{
	entropy_init(&entropy);

	if((ret=ctr_drbg_init(&ctr_drbg_ctx,entropy_func,&entropy,(const unsigned char*) pers,strlen(pers)))!=0) {
		printk(KERN_INFO "ctr_drbg_init() failed!\n");
		goto cleanup;
	}
	else {
		printk(KERN_INFO "ctr_drbg_init() success!\n");
	}
	
	if ((ret=ctr_drbg_random(&ctr_drbg_ctx,key,32))!=0) { /* key - is newly generated 256-bit AES key */
		printk(KERN_INFO "ctr_drbg_random() failed\n");
		goto cleanup;
	}
	else {
		printk(KERN_INFO "ctr_drbg_random succes!\n");
	}
	
	return SUCCESS;
	
	cleanup:
		ctr_drbg_free(&ctr_drbg_ctx);
	return 1;	
	
}

static void __exit fexit(void)
{
}

module_init(finit);
module_exit(fexit);
//traditionally in the end
MODULE_LICENSE("GPL");
MODULE_AUTHOR("thatskriptkid");
MODULE_DESCRIPTION("module that encrypts data with 256-bit AES key");
