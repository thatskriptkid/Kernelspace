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
#include "ksocket.h"

#define SUCCESS 0
#define REQUEST "GET /stefan/testfile.txt HTTP/1.1\r\nHost: thunked.org\r\n\r\n"

static struct socket *sockp;
__u32 		  ip;
int 		  port;
    
static int __init finit(void)
{
	ksock_create(&sockp,ip,port);
	/*	printk(KERN_WARNING "ksock_create() failed\n");
	else
		printk(KERN_WARNING "ksock_create() success\n");
		*/
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
