/*
  IOCTL Handler functions
  Created by Snehasish Banerjee
  Created on 15th June 2013
*/
#include "myd_common.h"
#include <linux/ioctl.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/spinlock.h>
#include "myd.h"

int g_exist = 0;
//myd_device_t dev;
spinlock_t ioctl_lock;

int myd_ioctl_handler(unsigned int ioctl_number, unsigned long arg)
{
	int ret = 0;
	char buffer[256];

	spin_lock (&ioctl_lock);
	
	switch (ioctl_number) {
	case IOCTL_NEW_DEVICE:
		printk (KERN_INFO "IOCTL called for new device registration.\n");
		memset (buffer, 0, sizeof (buffer));
		ret = copy_from_user(buffer, (void __user *)arg, 256);
		ret = ioctl_myd_create_device(buffer);
		/* Returning the major number of the device created for user space info */
		break;
	case IOCTL_DEL_DEVICE:
		printk (KERN_INFO "IOCTL called for deleting device\n");
		memset (buffer, 0, sizeof (buffer));
		ret = copy_from_user(buffer, (void __user *)arg, 256);
		ret =  ioctl_myd_remove_device (buffer); /* Success should return 0 */
		break;
	case IOCTL_DEL_ALL_DEVICES:
		printk (KERN_INFO "ICOTL called for deleting all devices\n");
		ioctl_myd_remove_all_devices ();
		break;
	case IOCTL_SAMPLE:
		memset (buffer, 0, sizeof (buffer));
		ret = copy_from_user(buffer, (void __user *)arg, 256);
		ioctl_sample(buffer);
		break;
	default:
		printk (KERN_INFO "Wrong IOCTL called\n");
	}
	spin_unlock(&ioctl_lock);
	return ret;
}
