/* Header file for mydriver */

/* Struture for a dummy ssd device */

/*
  struct block_device_operations {
	int (*open) (struct block_device *, fmode_t);
	int (*release) (struct gendisk *, fmode_t);
        int (*locked_ioctl) (struct block_device *, fmode_t, unsigned, unsigned long);
        int (*ioctl) (struct block_device *, fmode_t, unsigned, unsigned long);
        int (*compat_ioctl) (struct block_device *, fmode_t, unsigned, unsigned long);
        int (*direct_access) (struct block_device *, sector_t,
                                                  void **, unsigned long *);
        int (*media_changed) (struct gendisk *);
	int (*revalidate_disk) (struct gendisk *);
        int (*getgeo)(struct block_device *, struct hd_geometry *);
        struct module *owner;
  };*/

#define NODE_NAME "myssd"
#define CTL_NODE "/dev/myssd"

#define MISC_MAJOR 10

/* IOCTL number */
#define IOCTL_SAMPLE		_IOR (MISC_MAJOR, 0, char *)
#define IOCTL_NEW_DEVICE	_IOWR (MISC_MAJOR, 1, char *)
#define IOCTL_DEL_DEVICE	_IOWR (MISC_MAJOR, 4, char *)
#define IOCTL_DEL_ALL_DEVICES	_IO (MISC_MAJOR, 5)

#if 0
  /* mydriver device structure */
typedef struct myd_device_
{
	char name[32];
	int major_number;
	struct block_device_operations *fops;
	
} myd_device_t;
#endif
