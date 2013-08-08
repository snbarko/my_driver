/*
  Memory management
  mydriver
  Created by Snehasish Banerjee
  Date 23rd June 2013
*/
#include <linux/module.h>
#include <linux/mm_types.h>
#include <linux/slab.h>
#include <linux/gfp.h>
#include <linux/mm.h>

/* Memory allocation MACROs */
atomic64_t myd_page_count;
atomic64_t myd_kmalloc_count;
atomic64_t myd_vmalloc_count;

struct page* myd_alloc_pages(int gfp_mask, int order)
{
	struct page *my_page = alloc_pages(gfp_mask, order);
	if (my_page)
		atomic64_add(1<<order, &myd_page_count);
	else
		printk (KERN_CRIT "Page allocation failed\n");
	return my_page;
}

void myd_free_pages(struct page *my_page, int order)
{
	if (my_page) {
		__free_pages(my_page, order);
		atomic64_sub(1<<order, &myd_page_count);
	}
}

void* myd_kmalloc (int size, gfp_t flags)
{
	void *pointer = kmalloc (size, flags);
	if (pointer)
		atomic64_add(size, &myd_kmalloc_count);
	return pointer;
}

void myd_kfree(void *pointer)
{
	atomic64_sub(sizeof (pointer), &myd_kmalloc_count);
	kfree (pointer);
}

void* myd_vmalloc (int size)
{
	void *pointer = vmalloc (size);
	if (pointer)
		atomic64_add(size, &myd_vmalloc_count);
	return pointer;
}

void myd_vfree(void *pointer)
{
	atomic64_sub(sizeof (pointer), &myd_vmalloc_count);
	vfree (pointer);
}


struct page *my_page = NULL;

int myd_init_mem(void)
{
	void *buffer;
	printk(KERN_INFO "Initializing Memory..\n");

	my_page = myd_alloc_pages(GFP_KERNEL, 0);

	if (!my_page) {
		printk(KERN_ERR "Page Allocation failed.\n");
		goto out;
	}
	printk (KERN_INFO "Page is allocated address %p\n", my_page);

	buffer = page_address (my_page);

	memset (buffer, 0, PAGE_SIZE);

	sprintf (buffer, "This is page of 4K size. Hello Kernel\n");
out:
	return 0;
}

int myd_exit_mem(void)
{
	printk(KERN_INFO "Freeing up memory...\n");

	if (my_page) {
		myd_free_pages(my_page, 0);
		my_page = NULL;
		printk (KERN_INFO "Page is freed\n");
	}
	printk(KERN_INFO "Freed up memory...\n");
	return 0;
}
