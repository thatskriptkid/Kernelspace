/* polarssl headers */
#include "aes.h"
#include "entropy.h" 
#include "ctr_drbg.h"
#include "sha256.h"
/* polarssl headers */

#include "polarssl_kernel_support.h"

#define SUCCESS 0
#define KEY_LENGTH 32

/* AES section */

static 				  ctr_drbg_context ctr_drbg_ctx;
static 				  entropy_context entropy;
static unsigned char  key[32]; 										 /* hold 256-bits key     */
static unsigned char  *random_str  		= "wm82nZNB8FAfkqXMVD7G"; 	 /* our random string :)  */
static unsigned char  init_vector[16]   = "wm82nZNB8FAfkqXM";		 /* initialization vector */
static unsigned char  input[128];
static unsigned char  output[128];
static unsigned char  dec_output[128];

/* AES section */

/* SHA 256 section */ 

static unsigned char  hash_str[256];
static unsigned char  sha256_output[32];

/* SHA 256 section */ 

static int 			  AES_genkey(void);
static int 			  AES_enc(void);
static int			  AES_dec(void);
static int 			  SHA256(void);

static int __init finit(void)
{
	int error = -EINVAL;
	
	error = klog_init(KL_DBG_L);
	
	if (error) {
		printk(KERN_ERR "klog_init failed with err=%d", error);
		goto out;
	}
	
	SHA256();
	
	if(AES_genkey())
		printk(KERN_INFO "AES_genkey() failed\n");
	else
		printk(KERN_INFO "AES_genkey() success!\n");
		
	if(AES_enc())
		printk(KERN_INFO "AES_encrypt() failed\n");
	else
		printk(KERN_INFO "AES_encrypt() success!\n");
		
	if(AES_dec())
		printk(KERN_INFO "AES_dec() failed\n");
	else
		printk(KERN_INFO "AES_dec() success!\n");
	
	
	out:
		klog_release();
		
	return SUCCESS;
}

static int SHA256(void)
{
	unsigned char test[32]="ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad";
	
	memset(sha256_output,0,32);
	memset(hash_str,0,256);
	
	memcpy(hash_str,"abc",3);
	
	sha256(hash_str,256,sha256_output,0);
	
	printk(KERN_WARNING "%*phN",sha256_output);
	
	/*
	if(!strcmp(test,sha256_output))
		printk(KERN_WARNING "equal!\n");
	else
		printk(KERN_WARNING "not equal :(\n");
	*/
	return SUCCESS;
}

static int AES_dec(void)
{
	memcpy(init_vector,"wm82nZNB8FAfkqXM",16);
	
	memset(dec_output,0,128);
	
	if(aes_setkey_dec(&ctr_drbg_ctx.aes_ctx,key,256)!=0) {
		printk(KERN_WARNING "aes_setkey_dec() failed\n");
		return 1;
	}
	
	if(aes_crypt_cbc(&ctr_drbg_ctx.aes_ctx,AES_DECRYPT,KEY_LENGTH,init_vector,output,dec_output)!=0) {
		printk(KERN_WARNING "aes_crypt_cbc() failed!\n");
		return 1;
	}
	
	klog(KL_DBG,"decrypted string = %s\n",dec_output);
	return SUCCESS;
}
static int AES_enc(void)
{
	memset(output,0,128);
	
	memset(input,0,128);
	
	memcpy(input,"abcdefghiklmnoplqrstabcdefghiklmnoplqrst",40);
	
	if(aes_setkey_enc(&ctr_drbg_ctx.aes_ctx,key,256)!=0) {
		printk(KERN_WARNING "aes_setkey_enc() failed\n");
		return 1;
	}
	
	if(aes_crypt_cbc(&ctr_drbg_ctx.aes_ctx,AES_ENCRYPT,KEY_LENGTH,init_vector,input,output)!=0) {
		printk(KERN_WARNING "aes_crypt_cbc() failed!\n");
		return 1;
	}
	
	klog(KL_DBG,"encrypted string = %s\n",output);
	
	return SUCCESS;
}

static int AES_genkey(void)
{
	int ret; 
	
	entropy_init(&entropy);

	if((ret=ctr_drbg_init(&ctr_drbg_ctx,entropy_func,&entropy,(unsigned char*) random_str,strlen(random_str)))!=0) {
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
	ctr_drbg_free(&ctr_drbg_ctx);
}

module_init(finit);
module_exit(fexit);

/* traditionally in the end */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("thatskriptkid");
MODULE_DESCRIPTION("AES256 and SHA256 Polarssl realization");
