#pragma once
#include <linux/kernel.h>  // printk()
#include <linux/module.h>  // THIS_MODULE
#include <linux/slab.h>    // kmalloc()
#include <linux/string.h>
#include <linux/fcntl.h>
#include <asm/uaccess.h>
#include <linux/types.h>    //umode_t
#include <linux/fs.h>
#include "klog.h"
#include <asm/segment.h>
#include <linux/syscalls.h>

