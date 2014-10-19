/*#include "ssl.h"
#include "entropy.h"
#include "ctr_drbg.h"
#include "certs.h"
#include "x509.h"
#include "error.h"
#include "debug.h"
*/

#if !defined(POLARSSL_CONFIG_FILE)
#include "config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

#if defined(POLARSSL_LINUX_KERNEL)
#include "polarssl_kernel_support.h"
#include "ksocket.h"
#endif

#define SUCCESS 0

static struct socket *sockp = NULL;
static __u32 		  ip;
static int 		  port;
    
static int __init finit(void)
{
	int error = -EINVAL;
	
	error = klog_init(KL_DBG_L);
	
	if (error) {
		printk(KERN_ERR "klog_init failed with err=%d", error);
		goto out;
	}
	else 
		printk(KERN_ERR "klog_init success!", error);
	
	
	/*
	if(ksock_create(&sockp,ip,port))
		printk(KERN_WARNING "ksock_create() failed\n");
	else
		printk(KERN_WARNING "ksock_create() success\n");
	*/
	
	if(ksock_create(&sockp,ip,port))
		printk(KERN_WARNING "ksock_create() failed\n");
	else
		printk(KERN_WARNING "ksock_create() success\n");
		
	out:
		klog_release();
		
	ksock_release(sockp);
	
	return SUCCESS;
}

static void __exit fexit(void)
{
	
}

module_init(finit);
module_exit(fexit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("thatskriptkid");
MODULE_DESCRIPTION("SSL server using Polarssl library");

