/*
  mydriver - init driver
  Snehasish Banerjee
  Created on 14th June 2013
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/kthread.h>
//#include <linux/smp_lock.h>
#include "myd_common.h"
#include "myd.h"
#include "myd_debug.h"

extern int g_exist;
extern myd_device_t dev;
extern atomic64_t g_dev_count;
int start_process = 0;

module_param(start_process, int, S_IRUGO | S_IWUSR);

void timer_function(unsigned long data);


int read_device (struct inode *inode, struct file *file)
{
	printk (KERN_INFO "Reading device\n");
	return 0;
}

int ioctl_device (struct inode *inode, struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{
	return myd_ioctl_handler(ioctl_num, ioctl_param);
}

int unl_ioctl_device (struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{
	return myd_ioctl_handler(ioctl_num, ioctl_param);
}

static struct file_operations myd_fops = {
	.owner	= THIS_MODULE,
	.read = (void *)read_device,
	.unlocked_ioctl = (void *)unl_ioctl_device /* There is no ioctl call in 3.4.11 Kernel */
} ;

static struct miscdevice myd_device = {
	MISC_DYNAMIC_MINOR,
	NODE_NAME,
	&myd_fops
};

int register_main_device(void)
{
	int ret;

	if ((ret = misc_register(&myd_device)) < 0) {
		printk(KERN_ERR "Device registration failed ERR %d\n", ret);
		goto out;
	}
	printk (KERN_INFO "Device is registered. minor number of the device %d ret %d\n", myd_device.minor, ret);
out:
	return 0;
}

int unregister_main_device (void)
{
	misc_deregister(&myd_device);
	printk (KERN_INFO "Device is suucessfully unregistered\n");

	return 0;
}

extern spinlock_t ioctl_lock;

int thread_function (void *data)
{
	int i = 0;

	printk (KERN_INFO "Creation of thread function\n");

//	lock_kernel();
//	daemonize("myd_thrd");
//	siginitsetinv(&current->blocked, 0);
//	unlock_kernel();
	printk ("Monitor Thread started\n");
	while (i <10) {
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout_interruptible (30000);
		printk ("Monitor Thread running %d\n", i);
		i ++;
//		down_timeout()
	}
	printk (KERN_INFO "Thread going to exit\n");
	return 0;
}

void timer_function (unsigned long data)
{
/*	struct task_struct *ts = current, *ts1 = NULL;
 */
	printk ("This function is called after 10 ticks\n");
/*
	printk (KERN_INFO "This task : pid %s pid %d\n", ts->comm, ts->pid);

	ts1 = kthread_create (thread_function, NULL, "myd_process");
	if (!ts1) {
		printk (KERN_CRIT "Thread creating failed\n");
		goto out;
	}
	wake_up_process(ts1);
out:
	return;*/
}
struct task_struct *ts1 = NULL;

struct timer_list my_timer;

static int driver_init(void)
{
//	struct task_struct *ts = current, *ts1 = NULL;
	int bitmap = 0;


	set_bit (1, (unsigned long *)&bitmap);

	printk ("bitmap is %d\n", bitmap);

	printk (KERN_INFO "testbit is %d\n", test_bit (2, (unsigned long *)&bitmap));
	printk (KERN_INFO "Initializing the Kernel Driver\n");

	/* Init Proc Interface for the module */
	myd_init_proc();

	/* Control Device for the Kernel Module */
	register_main_device();

	myd_init_mem();
	myd_init_disk_structure();

	/* Introduce a lock in each module */
	spin_lock_init(&ioctl_lock);
#if 0
	/* Init a timer */
	init_timer (&my_timer);

	my_timer.expires = jiffies + 10000; /* 10000 is 10 seconds.. jiffies in milliseconds */
	my_timer.data = 0;
	my_timer.function = timer_function;
	add_timer(&my_timer);
//	printk (KERN_INFO "jiffies : %lu\n", jiffies);


	//printk (KERN_INFO "This task : pid %s pid %d\n", ts->comm, ts->pid);
/*	ts1 = kthread_create (thread_function, NULL, "myd_process");
	if (!ts1) {
	printk (KERN_CRIT "Thread creating failed\n");
	goto out;
	}
	wake_up_process(ts1);*/
#endif

	return 0;
}

static void driver_exit (void)
{
	if (atomic64_read(&g_dev_count)) {
		printk (KERN_ERR "Cannot unload the module. Devices exist\n");
		return;
	}

	/* Remove proc interface */
	myd_exit_proc();

	myd_exit_mem();

/*	myd_remove_device(&dev);*/ // Remove all the devices before signing out
	unregister_main_device ();

	if (ts1) {
//		wake_up(ts1);
		kthread_stop(ts1);
	}

	printk (KERN_INFO "Removing the Kernel Driver\n");
}

module_init (driver_init);
module_exit (driver_exit);

#ifdef OWNER
MODULE_AUTHOR("Snehasish Banerjee");
#endif
MODULE_DESCRIPTION("Kernel Driver Development Learning module");
MODULE_LICENSE("GPL");
MODULE_VERSION ("1.0.0");


