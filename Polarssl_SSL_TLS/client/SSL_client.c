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

#include <linux/inet.h>
#include <linux/byteorder/generic.h> /* Generic Byte-reordering support */

#define SUCCESS 0
#define REQUEST "GET /stefan/testfile.txt HTTP/1.1\r\nHost: thunked.org\r\n\r\n"

static struct socket 	  *sockp = NULL;
static __u32 		  	   ip;
static int			  	   port;
static char				  *buff="1234567890";
static int				  *pw;

static int __init finit(void)
{
	int error = -EINVAL;
	
	error = klog_init(KL_DBG_L);
	
	if (error) {
		printk(KERN_ERR "klog_init failed with err=%d", error);
		goto out;
	}
	else 
		printk(KERN_ERR "klog_init success!");
	
	
	if(ksock_create(&sockp,0,0)) {//create socket without binding
		printk(KERN_ERR "ksock_create() failed\n");
		goto out_sock;
	}
	else 
		printk(KERN_ERR "ksock_create() success\n");
		
	ip = ntohl(in_aton("127.0.0.1"));
	port =9900;
	
	if(ksock_connect(&sockp,0,0,ip,port)) {
		printk(KERN_ERR "ksock_connect() failed\n");
		goto out_sock;
	}
	else 
		printk(KERN_ERR "ksock_connect() success\n");
	
	error = ksock_write_timeout(sockp,buff, 10,1, pw);
	
	printk(KERN_ERR "ksock_write error = %d",error);
	
	out_sock:
		ksock_release(sockp);	
	out:
		klog_release();
		
	return SUCCESS;
}

static void __exit fexit(void)
{
	
}

module_init(finit);
module_exit(fexit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("thatskriptkid");
MODULE_DESCRIPTION("SSL client using Polarssl library");


