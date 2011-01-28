#include <linux/fs.h>       //Files operations.
#include <linux/device.h>   //For allocating device numbers.
#include <linux/major.h>    //Device major numbers definitions. 
#include <linux/module.h>   
#include <linux/init.h>
#include <linux/cdev.h>

/******************************************************************************/
//Local variables definitions.


#define MODULE_NAME "gpiochar_dev"

MODULE_AUTHOR("marco v");
MODULE_LICENSE("GPL");

struct privdata_s {
	char * buf;
	int count;
};

// This structure is defined for pure convenience.
struct gpiochar_s {
  struct cdev cdev;
  struct privdata_s priv;
  unsigned int count;
  uint32_t major;
  uint32_t minor;
  char * name;
};
  
struct gpiochar_s gpiochar = {
	.count = 1,
	.minor = 0,
	.name = MODULE_NAME,
};

int gpiochar_open(struct inode *inode, struct file *filp);
int gpiochar_release(struct inode *inode, struct file *filp);
ssize_t gpiochar_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos);
ssize_t gpiochar_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos);


// File operations structure
struct file_operations gpiochar_fops = {
	.owner = THIS_MODULE,
	.read   = gpiochar_read,
	.write  = gpiochar_write,
	//.ioctl  = gpiochar_ioctl,
	.open   = gpiochar_open,
	.release = gpiochar_release,
};




/***************************************************************************//**
 * @brief  
 *
 * */
int gpiochar_open(struct inode *inode, struct file *filp)
{
	filp->private_data = &(gpiochar.priv);
	printk (KERN_WARNING "Requested %s", __func__);
	return 0;
}


/***************************************************************************//**
 * @brief  
 *
 * */

int gpiochar_release(struct inode *inode, struct file *filp)
{
		return 0;
}


/***************************************************************************//**
 * @brief  
 *
 * */

ssize_t gpiochar_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos)
{
int retcnt = 0;
struct privdata_s * priv = filp->private_data;

	printk (KERN_WARNING "Received request to read for size %d", count);

	if (priv->count > 0) {
		copy_to_user(buf, priv->buf, priv->count);
		retcnt = count;
		kfree(priv->buf);
		*f_pos +=count;
		priv->count = 0;
	}

	return retcnt;
}


/***************************************************************************//**
 * @brief  
 *
 * */

ssize_t gpiochar_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
struct privdata_s * priv = filp->private_data;

	priv->buf = kmalloc(count, GFP_KERNEL);
	copy_from_user(priv->buf, buf, count);
	priv->count = count;
	printk (KERN_WARNING "Received request to write for size %d", count);
	*f_pos +=count;
	return count;
}


/***************************************************************************//**
 * @brief  This function create the character device.
 *
 * */
static int cdev_setup(struct cdev * cdev)
{
int err, devno = MKDEV(gpiochar.major, gpiochar.minor);

	cdev_init(cdev, &gpiochar_fops);
	cdev->owner = THIS_MODULE;
	cdev->ops = &gpiochar_fops;
	err = cdev_add(cdev, devno, 1);
	if(err)
		printk(KERN_NOTICE "Error %d adding gpiochar_dev%d", err, 0);
	
	return 0;
}

/***************************************************************************//**
 * @brief  Device driver initialization
 *
 * */
int gpio_drv_init(void)
{
int res = 0;
dev_t dev;

	res = alloc_chrdev_region(&dev, gpiochar.minor,
							gpiochar.count, gpiochar.name);
	
	gpiochar.major = MAJOR(dev);

	if (res < 0) {
		printk(KERN_WARNING "Can't allocate gpio char dev\n");
		return res;
	} else {
		printk(KERN_WARNING "Allocated gpiochar_dev with major %d and minor %d",
							gpiochar.major, gpiochar.minor);
	}
	cdev_setup(&(gpiochar.cdev));

	return 0;
}


/***************************************************************************//**
 * @brief Device driver cleanup.
 *
 * */
void gpio_drv_exit(void)
{
dev_t devno = MKDEV(gpiochar.major, gpiochar.minor);

	if(gpiochar.priv.count > 0){
		kfree(gpiochar.priv.buf);
	}
	cdev_del(&(gpiochar.cdev));
	unregister_chrdev_region(devno, gpiochar.count);
	
	printk(KERN_INFO "gpiochar_dev correctly removed\n");
}

module_init(gpio_drv_init);
module_exit(gpio_drv_exit);
