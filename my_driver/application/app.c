/*
  Application for interaction with the driver
  Created by SNehasish Banerjee
  Created on 14th June 2013
*/
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/fcntl.h>
#include <string.h>
#include "myd_common.h"

int main(int argc, char *argv[])
{
	int file_desc = -1;
	int ret = -1;
	char message[256];
	int i = 0;
	int operation = -1;
	char dev_name[32];
	char command[256];
	int major_number = -1;
	int opt2= 0;

	if (argc == 1) {
		printf ("Usage -t [operation - ioctl/create/open/dlnod] -s [string - \"hello\"/create/delete] -n [name of device-\"dev_name\"] -l [major_number of device]\n");
		printf ("Example: \napp -t ioctl -s create -n myssd\n");
		printf ("app -t ioctl -s delete -n myssd\n");
		printf ("app -t ioctl -s \"hello\"\n");
		printf ("app -t open\n");
		return 0;
	}

	memset (message, 0, sizeof (message));
	memset (dev_name, 0, sizeof (dev_name));
	i = 1;
	while (i < argc) {
/*		if (!argv[i+1]){
			break;
			}*/ // FIXME : Validate 2nd armument for options with 2nd value

		if (strcmp (argv[i], "-t") == 0) {
			if (strcmp(argv[i+1], "ioctl") == 0) {
				operation = 0;
			} else if (strcmp(argv[i+1], "open") == 0) {
				operation = 1;
			} else if (strcmp(argv[i+1], "create") == 0) {
				strcpy (message, "create");
				operation = 0;
			} else if (strcmp(argv[i+1], "delete") == 0) {
				operation = 0;
				strcpy (message, "delete");
			}
		} else if (strcmp (argv[i], "-s")  == 0) {
			strcpy (message, argv[i+1]);
		} else if (strcmp (argv[i], "-n") == 0) {
			strcpy (dev_name, argv[i+1]);
		} else if (strcmp (argv[i], "-l") == 0 ) {
			major_number = atoi (argv[i+1]);
		} else if (strcmp (argv[i], "-a") == 0 ) {
			opt2 = 1;
		} else {
			printf ("Invalid option\n");
			return -1;
		}
//		printf ("debug i %d argc %d\n", i ,argc);
		i+=2;
	}
	if (operation == -1) {
		printf ("Invalid operation entered.\n");
		return -1;
	}

	if (operation == 0) {
		if ((file_desc = open (CTL_NODE, 0)) < 0) {
			printf ("Device open failed\n");
			return -1;
		}

		if (!strcmp (message, "create")) {
			if (!strlen(dev_name)) {
				printf ("Device name is not provided\n");
				goto out;
			}
			printf ("Calling create device - %s\n", dev_name);
			major_number = ioctl (file_desc, IOCTL_NEW_DEVICE, dev_name); /* In this case return is the major number expected */
			printf ("Major number of the device returned is %d\n", major_number );
			if (major_number > 0) {
				printf ("Device node added at Kernel - Major Number %d\n", major_number);
/*				memset (command, 0, sizeof (command));
				sprintf (command, "mknod -m 660 /dev/%s b %d %d", dev_name, major_number, 0);
				ret = system (command);
				if (ret != 0 ) {
				printf ("Command failed. Recheck command: You provided: %s\n", command);
				goto mainout;
				} else {
				printf ("Device node created at /dev/%s\n", dev_name);
				}*/
			}
		} else if (!strcmp (message, "delete")) {
			if (!strlen(dev_name) && opt2 == 0) {
				printf ("Device name is not provided or proper option not selected\n");
				goto out;
			}
			printf ("Calling delete device\n");
			if (opt2  == 1) {
				ret = ioctl (file_desc, IOCTL_DEL_ALL_DEVICES, dev_name);
				if  (ret == 0) {
					printf ("All devices removed successfully\n");
				} else {
					printf ("ICOTL failed\n");
				}
			} else {
				ret = ioctl (file_desc, IOCTL_DEL_DEVICE, dev_name);
				if (ret == 0) {
					//				memset (command, 0, sizeof (command));
					//sprintf (command, "rm -f /dev/%s", dev_name);
					//ret = system (command);
					//if (ret != 0 ) {
					//		printf ("Command failed. Recheck Command: You provided: %s\n", command);
					//		goto mainout;
					//}
					printf ("Device removed successfully\n");
				} else {
					printf ("Device removal failed\n");
				}
			}
		} else {
			if (!strlen(message)) {
				printf ("No message providede at command line.. using default message \"hello kernel from user\"\n");
				sprintf (message, "hello kernel from user");
			}
			ret = ioctl (file_desc, IOCTL_SAMPLE, message);
		}
		printf ("IOCTL called ret %d\n", ret);
	out:
		close (file_desc);
	} else if (operation == 1) {
		//int filde = -1;
		FILE *fp = NULL;

		if (!strlen(dev_name)) {
			printf ("Device name is not provided\n");
			goto mainout;
		}

/*		filde = open (dev_name, O_RDONLY, O_DIRECT);
		if (filde < 0) {
		printf ("Device open failed.\n");
		goto mainout;
		}
//		sleep(10);
if (filde)
close (filde);*/
		fp = fopen (dev_name, "r");
		if (!fp) {
			printf("fopen failed.\n");
			goto mainout;
		}
		fclose (fp);

	}
	printf ("Success.\n");
mainout:
	return 0;
}
