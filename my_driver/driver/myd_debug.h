/* Debug print */

//#define MYD_PRINT(x, y...) printk(x,##y)
#ifdef DEBUG
	#define DBG		KERN_ALERT
	#define MYD_DBG		printk
#else
	#define DBG
	#define MYD_DBG
#endif

/* LOG LEVELS */
int log_level = 1;
module_param (log_level, int, S_IRUGO | S_IWUSR);

#define MYD_ERR KERN_ERR
#define MYD_INFO KERN_INFO
/*
static void MYD_PRINT(int x, char *arg...)
{
	if (log_level == 8) {
		printk (x,##arg);
	}
	if (x == MYD_IOCTL) {
		
	}
}
*/
