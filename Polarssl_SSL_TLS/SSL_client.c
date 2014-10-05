#include "net.h"
//#include "ssl.h"

#define SUCCESS 0
#define REQUEST "GET /stefan/testfile.txt HTTP/1.1\r\nHost: thunked.org\r\n\r\n"


static int __init finit(void)
{
	int sock;
	int ret;
	
	if((ret=net_connect(&sock,"thunked.org",443))!=0)
		printk(KERN_WARNING "net_connect() failed\n");
	else
		printk(KERN_WARNING "net_connect() success\n");
		
	return SUCCESS;
}

static void __exit fexit(void)
{
	
}

module_init(finit);
module_exit(fexit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("thatskriptkid");
MODULE_DESCRIPTION("SSL client");
