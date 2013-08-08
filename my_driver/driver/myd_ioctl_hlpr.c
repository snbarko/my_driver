/*
  IOCTL Helper functions to handle the function calls from IOCTL
  Created by Snehasish Banerjee
  Date: 25th JUNE 2013
*/
#include <linux/module.h>
#include <linux/mm.h>
#include "myd.h"

/* Have a global page queue (or list) to handling and freeing the pages at the module unloading.. */

/*
int page_allocation()
{
	
}*/

void ioctl_myd_remove_all_devices(void)
{
	/* Release all devices */
	myd_remove_all_devices();
}

int ioctl_myd_create_device(char *dev_name)
{
	/* Add new device structure */
	return myd_create_device(dev_name);
}

int ioctl_myd_remove_device(char *dev_name)
{
	/* Check in the list to get the device with particular dev_name */
	return myd_remove_device(dev_name);
}

extern struct page *my_page;

void ioctl_sample(char *message)
{
	void *buffer;

	buffer = page_address (my_page);
	printk(KERN_INFO "Sent new message from User: %s\n", message);
	printk (KERN_INFO "OLDER data at page : %s\n", (char *)buffer);
	memset (buffer, 0, PAGE_SIZE);
	sprintf (buffer, "%s", message);
}
