/*
  myd_device.c
  Device Creation/Deletion/Operations
  Created by Snehasish Banerjee
  Date - 18th JUNE 2013
*/
#include <linux/fs.h>
#include <linux/blkdev.h>
//#include <linux/export.h>
#include <linux/genhd.h>
#include <linux/slab.h>
#include <linux/module.h>
#include "myd_common.h"
#include "myd.h"
#include <linux/spinlock.h>
#include <linux/vmalloc.h>

void clean_device_structure(myd_device_t *device);

struct list_head g_dev_list;
atomic64_t g_dev_count;

int g_page_allocation_per_device = 2;
int g_page_count = 25600;
module_param(g_page_count, int, S_IRUGO | S_IWUSR);

atomic64_t g_read_io;
atomic64_t g_write_io;

int myd_device_opened (struct inode *inode, struct file *filp)
{
	return 0;
}

int myd_device_released (struct inode *inode, struct file *filp)
{
	return 0;
}

static struct block_device_operations myd_def_op = {
	.owner = THIS_MODULE,
	.open = (void *)myd_device_opened,
	.release = (void *)myd_device_released,
};

void  myd_queue_request2(struct request_queue *qreq, struct bio *bio)
{
	struct bio_vec *bvec;
	myd_device_t *myd_dev;
	int page_index, offset;

	myd_dev = qreq->queuedata;

	bio_rw(bio) == WRITE ? atomic64_add(bio_sectors(bio), &g_write_io) : atomic64_add(bio_sectors(bio), &g_read_io);

#if 0
	printk (KERN_INFO "Device structure - name %s major_number %d\n", myd_dev->name, myd_dev->major_number);
	printk (KERN_INFO "BIO - sector %llu bdev %p type %s vcnt %u bi_idx %u bi_phy_segments %u bi_size %u bi_cnt %d bi_max_vecs %unext %p \n", bio->bi_sector, bio->bi_bdev, bio_rw(bio) == WRITE ? "write" : "read", bio->bi_vcnt, bio->bi_idx, bio->bi_phys_segments, bio->bi_size, atomic_read(&bio->bi_cnt), bio->bi_max_vecs, bio->bi_next);

	// Testing the bio usage field from bio->bi_cnt field
	bio_get(bio);
	printk (KERN_INFO "%llu Now the usage is %u\n", bio->bi_sector, atomic_read(&bio->bi_cnt));
	bio_put (bio);
	printk (KERN_INFO "%llu Now the usage is %u\n", bio->bi_sector, atomic_read(&bio->bi_cnt));

	//Manipulating BIO structure without MACROs is not proper for Kernel's interest as developer can change the bio structure
	for (i = 0; i < bio->bi_vcnt; i++) {
		printk (KERN_INFO "PAGE %llu length %u offset %u\n", bio->bi_io_vec->bv_page, bio->bi_io_vec->bv_len, bio->bi_io_vec->bv_offset);
	}
#endif
	bio_for_each_segment (bvec, bio, bio->bi_idx) {
		char *buffer, *page_buffer;
		//	printk (KERN_INFO "BIO bvec page %p length %u offset %u\n", bvec->bv_page, bvec->bv_len, bvec->bv_offset);
		buffer = page_address (bvec->bv_page);
		page_index = bio->bi_sector / 8;
		//printk (KERN_INFO "%s sector %llu Page index at dev is %d\n", bio_rw(bio) == WRITE ? "WRITE" : "READ", bio->bi_sector, page_index);
		page_buffer = myd_dev->data;
		offset = bio->bi_sector * 512;
		if (bio_rw(bio) == WRITE) {
			memcpy (page_buffer + offset, buffer + bvec->bv_offset, bvec->bv_len);
		} else {
			memcpy (buffer + bvec->bv_offset , page_buffer +  offset , bvec->bv_len);
		}
	}

	bio_endio(bio, 0);
	return;
}

int myd_create_device(char *dev_name)
{
	int major_number;
	int ret = 0;
	myd_device_t *device;

	/*Allocation of a new device structure */
	device = kmalloc (sizeof (myd_device_t), GFP_KERNEL); /* FIXME: Change kmalloc to myd_kmalloc */
	if (!device)
	{
		printk(KERN_ERR "Memory allocation failed for device allocation\n");
		ret = -ENOMEM;
		goto out;
	}
	memset (device, 0, sizeof (myd_device_t));
	strncpy (device->name, dev_name, strlen (dev_name));

	if ((major_number = register_blkdev(0, device->name)) < 0) {
		printk (KERN_ERR "Device creation failed\n");
		ret = -1;
		goto out;
	}
	device->major_number = major_number;
	ret = major_number; /* Store the major_number in ret to send back to user via IOCTL */
	printk (KERN_INFO "Device %s is created with major number %d\n", device->name, device->major_number);

	device->fops = &myd_def_op;

	/* Allocation of memory for storage */
	device->data=myd_vmalloc (g_page_count * PAGE_SIZE);

	if (!device->data) {
		printk (KERN_CRIT "Vmalloc allocation failed\n");
		unregister_blkdev(device->major_number, device->name);
		ret = -ENOMEM;
		goto out;
	}

	device->no_pages = g_page_count;
	/* Add the device list to global list */
	/* FIXME : Put proper list initialization method */
	list_empty(&device->d_list);
	list_add_tail (&device->d_list, &g_dev_list);
	device->gd = alloc_disk(1);
	if (!device->gd) {
		printk (KERN_ERR "Alloc disk failure\n");
		unregister_blkdev(device->major_number, device->name);
		ret = -1;
		goto out;
	}
	device->gd->major = major_number;
	device->gd->first_minor = atomic64_read(&g_dev_count);
	strcpy(device->gd->disk_name, device->name);
	device->gd->fops  = &myd_def_op;
	set_capacity(device->gd, (g_page_count * PAGE_SIZE )/ 512);
	spin_lock_init(&device->d_lock);
	device->d_queue = blk_alloc_queue(GFP_KERNEL);
	if (!device->d_queue) {
		printk (KERN_INFO "INIT queue failed*****\n");
		unregister_blkdev(device->major_number, device->name);
		ret = -1;
		goto out;
	}
	device->d_queue->queuedata = device;
	if (device->d_queue)
		device->gd->queue = device->d_queue;
	blk_queue_make_request (device->d_queue, myd_queue_request2); /* Handling of queue request with BIO */
	device->gd->private_data = device;
	add_disk (device->gd);

	device->proc_dir = myd_create_device_proc (device->name);
		
	atomic64_inc(&g_dev_count);
out:
	return ret;
}
/* removal of all devices */
void myd_remove_all_devices(void)
{
	myd_device_t *device = NULL, *temp = NULL;
	
	/* FIXME: May need semaphore or spinlock to ensure that IO process is not in progress */
	/* NOTE - Use list_for_each_entry_safe with extra variable temp to hold the previous list_head during del_list */
	list_for_each_entry_safe(device, temp, &g_dev_list, d_list) { //List Clearance needed
		if (device) {
			printk (KERN_INFO "Deleting device %s\n", device->name);
			clean_device_structure(device);
			list_del (&(device->d_list));
			kfree (device);
		}
	}
	printk (KERN_INFO "Remove all devices done\n");
}


/* Remove Single device */
int myd_remove_device (char *name)
{
	myd_device_t *device;
	int ret = -1;

	printk (KERN_INFO "%s Device Removal called name - %s\n", __FUNCTION__, name);
	/* NOTE - list_for_each_entry Valid as the list loop is going the last time only when del_list is done */
	/* Before removing a device check in the list about the presence of the device */
	list_for_each_entry (device, &g_dev_list, d_list) {
		if (strcmp(device->name, name) == 0) {
			ret = 0;
			printk (KERN_INFO "Device found: Name: %s Major: %d\n", device->name, device->major_number);
			list_del (&(device->d_list));
			clean_device_structure(device);
			kfree (device);
			break;
		}
	}

	return 0;
}

/* Cleaning device structure */
void clean_device_structure(myd_device_t *device)
{
	myd_vfree (device->data);
	del_gendisk (device->gd);
	unregister_blkdev(device->major_number, device->name);
	myd_delete_device_proc(device->proc_dir, device->name);
	atomic64_dec(&g_dev_count);
}

int myd_init_disk_structure(void)
{
	/* Initialize the disk list when no device is present */
	atomic64_set(&g_dev_count, 0);
	INIT_LIST_HEAD(&g_dev_list);
	return 0;
}
