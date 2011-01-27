/*

Template for a character device driver for a PCI device,
based on the templates from the book "Linux Treiber entwickeln" and
"Linux Device Drivers".

2 in 1 driver: PCI device driver (incl. ISR) +character device driver.

For a real driver replace _template by the name of your hardware, e. g. _foobar,
and fill in the correct vendor and device id.
Makefile and pci_template.sh have to be edited similar (only one replacment per file).

For the "kernel: pci_template: unsupported module, tainting kernel." kernel
message from loading the Module see
http://www.novell.com/coolsolutions/feature/14910.html

Rolf Freitag 2005

*/
/* This program is free software; you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation; either version 2 of the License, or    */
/* (at your option) any later version.                                  */
/*                                                                      */
/* This program is distributed in the hope that it will be useful,      */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         */
/* GNU General Public License for more details.                         */
/*                                                                      */
/************************************************************************/
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/major.h>
#include <linux/module.h>                         // MOD_DEVICE_TABLE,
#include <linux/init.h>
#include <asm/uaccess.h>                          // copy_to_user,
#include <linux/version.h>                        // KERNEL_VERSION,

// simple version number
#define _template_SOFTWARE_VERSION_NUMBER 1.0

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)    /* not < 2.5 */
#  error "This kernel is too old: not supported by this file"
#endif

static int i_foo;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("marco v");
MODULE_DESCRIPTION("Sigma tango2 gpio dev driver");
module_param(i_foo, int, 0444);                   // Module parameter, unused
MODULE_PARM_DESC(i_foo, "foo bar");         // unused


// declarations for fops
static int _template_open (struct inode *, struct file *);
static int _template_close (struct inode *, struct file *);
static ssize_t _template_read (struct file *, char *, size_t, loff_t *);
static ssize_t _template_write (struct file *, __user const char *, size_t, loff_t *);

static int __init gpio_drv_init(void);
static void __exit gpio_drv_exit(void);

module_init(gpio_drv_init);
module_exit(gpio_drv_exit);


uint8_t gpio_major = 0;
static struct class *gpio_dev_class;
struct device *dev;

static struct file_operations _template_fops =
{
  .owner = THIS_MODULE,
  .read = _template_read,
  .write = _template_write,
  .open = _template_open,
  .release = _template_close,
};


static ssize_t show_my_name(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "This is my name nigga!\n");
}
static DEVICE_ATTR(name, S_IRUGO, show_my_name, NULL);

// device driver init
static
int __init gpio_drv_init(void)
{
int res = 0;

	printk (KERN_CRIT "gpio_chrdev: initialising\n");
	if (register_chrdev (gpio_major, "gpio_chrdev", &_template_fops) < 0)
	{
    	printk (KERN_CRIT "gpio_chrdev: Cannot register device.\n");
    	return (-1);
	}
		
	gpio_dev_class = class_create(THIS_MODULE, "gpio-dev");	
	if (IS_ERR(gpio_dev_class))		return -1;

    /* register this i2c device with the driver core */
	device_create(gpio_dev_class, &dev,
	              MKDEV(gpio_major, 0),
	              "mygpio-%d", 0);

	if (IS_ERR(dev)) return -1;
									    
	res = device_create_file(dev, &dev_attr_name);
    if (res) return -1;


  return 0;
}


static
void __exit gpio_drv_exit(void)
{
  unregister_chrdev(gpio_major,"gpio_chrdev");
  return;
}


static int
_template_open (struct inode *inode, struct file *filp)
{
  return (0);
}


static int
_template_close (struct inode *inode, struct file *filp)
{
  return (0);
}


static ssize_t
_template_read (struct file * filp, char *buff, size_t count, loff_t * ppos)
{
  printk(KERN_DEBUG "Called function %s", __func__);
  return 0;
}


static ssize_t
_template_write (struct file * filp, __user const char *buff, size_t count, loff_t * ppos)
{
  printk(KERN_DEBUG "Called function %s", __func__);
  return 0;
}
