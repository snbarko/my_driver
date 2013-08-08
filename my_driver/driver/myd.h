/* Driver based header file having functions and structures */

//#ifndef __myd_header__
//#define __myd_header__

/* mydriver device structure */
typedef struct myd_device_
{
	char name[32];
	int major_number;
	struct block_device_operations *fops;
	struct list_head d_list;
	struct gendisk *gd;
	spinlock_t d_lock;
	struct request_queue *d_queue;
//	struct page *page_data;
	void *data;
	int no_pages;
	struct proc_dir_entry *proc_dir;
} myd_device_t;

/* Function prototypes */
int myd_ioctl_handler (unsigned int ioctl_number, unsigned long arg);

/* Proc Functions */
void myd_init_proc (void);
void myd_exit_proc (void);
struct proc_dir_entry *  myd_create_device_proc (char *name);
void myd_delete_device_proc (struct proc_dir_entry *dev_dir, char *name);
/* Proc Functions */

int myd_init_mem (void);
int myd_exit_mem (void);
int myd_remove_device (char *dev_name);
int myd_create_device(char *dev_name);
void myd_remove_all_devices(void);
int myd_init_disk_structure(void);
struct page* myd_alloc_pages(int gfp_mask, int order);
void myd_free_pages(struct page *my_page, int order);
void* myd_vmalloc (int size);
void myd_vfree(void *pointer);
/* IOCTL helper functions */
int ioctl_myd_create_device(char *dev_name);
int ioctl_myd_remove_device(char *dev_name);
void ioctl_myd_remove_all_devices(void);
void ioctl_sample(char *message);
/* End of IOCTL helper functions */

//#endif
