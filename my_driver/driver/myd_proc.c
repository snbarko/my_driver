/*
  Proc Interface for mydriver
  Created by Snehasish Banerjee
  Date : 20 June 2013
*/
#include <linux/proc_fs.h>
//#include <linux/export.h>
#include <linux/list.h>
#include "myd.h"

extern atomic64_t myd_page_count;
extern atomic64_t myd_kmalloc_count;
extern atomic64_t myd_vmalloc_count;
extern struct list_head g_dev_list;
extern atomic64_t g_read_io;
extern atomic64_t g_write_io;

/*
  NOTE: Proc read or write buffer is limited to PAGE_SIZE i.e., 4KB
*/
int read_data_c (char *page, char **start, off_t offset, int count, int *eof, void *data)
{
	int len = 0;
	myd_device_t *dev;

	len+= sprintf (page+len, "This is proc interface\n");
	len+= sprintf (page+len, "Pages allocated: %lld\n", atomic64_read(&myd_page_count));
	len+= sprintf (page+len, "Kmalloc: %lld\n", atomic64_read (&myd_kmalloc_count));
	len+= sprintf (page+len, "Vmalloc: %lld\n", atomic64_read (&myd_vmalloc_count));
	len+= sprintf (page+len, "READ IO : %llu sectors WRITE IO : %llu sectors\n", atomic64_read(&g_read_io), atomic64_read(&g_write_io));
	len+= sprintf(page+len, "Devices available:\n");
	list_for_each_entry(dev, &g_dev_list, d_list) {
		len+= sprintf(page+len, "Dev : Name %s Major Number %d\n", dev->name, dev->major_number);
	}
	
	*eof = 1;
	return len;
}

struct proc_dir_entry *parent  = NULL;

struct proc_dir_entry * myd_create_device_proc (char *name)
{
	struct proc_dir_entry *dev_dir;
	struct proc_dir_entry *entry;
		
	if (!parent) {
		printk (KERN_ERR "Proc Creation failed\n");
		return NULL;
	}
	dev_dir = proc_mkdir(name, parent);
	
	entry = create_proc_entry("info", 0, dev_dir);
	entry->data = NULL;
	entry->read_proc = NULL;
	entry->write_proc = NULL;

	return dev_dir;
}

void myd_delete_device_proc (struct proc_dir_entry *dev_dir, char *name)
{
	if (dev_dir) {
		remove_proc_entry("info", dev_dir);
		remove_proc_entry(name, parent);
	}
}

void myd_init_proc(void)
{
	struct proc_dir_entry *entry;

	/* FIXME  : Have a proc lock here for no racing condition during read and write */
	/* FIXME: Not GOOD explanation in 'device driver' book.. so skipping with basic design as of now */
	parent = proc_mkdir("mydriver", NULL); /*Should use (THIS_MODULE)->name in place of "mydriver" */

	if (!parent) {
		printk (KERN_ERR "Proc Creation failed\n");
		return;
	}

	entry = create_proc_entry ("info", 0, parent);
	entry->data = NULL;
	entry->read_proc = read_data_c;
	entry->write_proc = NULL;

	entry = create_proc_entry ("debug", 0, parent);
	entry->data = NULL;
	entry->read_proc = NULL; /* debug */
	entry->write_proc = NULL;
}

void myd_exit_proc (void)
{
	if (parent) {
		remove_proc_entry("info", parent);
		remove_proc_entry("debug", parent);
		remove_proc_entry("mydriver", NULL);
	}

}

