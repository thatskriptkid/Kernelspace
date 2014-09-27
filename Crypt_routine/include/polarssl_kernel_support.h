#pragma once 

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
#include <linux/types.h>
#include "klog.h"

#define __SUBCOMPONENT__ "mycrypt"
#define __LOGNAME__ "mycrypt.log"

