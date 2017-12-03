#include <linux/fs.h>       //Files operations.
#include <linux/device.h>   //For allocating device numbers.
#include <linux/major.h>    //Device major numbers definitions.
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>

/******************************************************************************/
//Local variables definitions.


#define MODULE_NAME "template_chardev"

MODULE_AUTHOR("marco v");
MODULE_LICENSE("GPL");

struct privdata_s {
	char * buf;
	int count;
};

// This structure is defined for pure convenience.
struct tchardev_s {
  struct cdev cdev;
  struct privdata_s priv;
  unsigned int count;
  uint32_t major;
  uint32_t minor;
  char * name;
};

struct tchardev_s tchardev = {
	.count = 1,
	.minor = 0,
	.name = MODULE_NAME,
};

int tchardev_open(struct inode *inode, struct file *filp);
int tchardev_release(struct inode *inode, struct file *filp);
ssize_t tchardev_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos);
ssize_t tchardev_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos);


// File operations structure
struct file_operations tchardev_fops = {
	.owner = THIS_MODULE,
	.read   = tchardev_read,
	.write  = tchardev_write,
	//.ioctl  = tchardev_ioctl,
	.open   = tchardev_open,
	.release = tchardev_release,
};




/***************************************************************************//**
 * @brief
 *
 * */
int tchardev_open(struct inode *inode, struct file *filp)
{
	filp->private_data = &(tchardev.priv);
	printk (KERN_WARNING "Requested %s", __func__);
	return 0;
}


/***************************************************************************//**
 * @brief
 *
 * */

int tchardev_release(struct inode *inode, struct file *filp)
{
		return 0;
}


/***************************************************************************//**
 * @brief
 *
 * */

ssize_t tchardev_read(struct file *filp, char __user *buf, size_t count,
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

ssize_t tchardev_write(struct file *filp, const char __user *buf, size_t count,
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
int err, devno = MKDEV(tchardev.major, tchardev.minor);

	cdev_init(cdev, &tchardev_fops);
	cdev->owner = THIS_MODULE;
	cdev->ops = &tchardev_fops;
	err = cdev_add(cdev, devno, 1);
	if(err)
		printk(KERN_NOTICE "Error %d adding %s - err=%d", MODULE_NAME, err, 0);

	return 0;
}

/***************************************************************************//**
 * @brief  Device driver initialization
 *
 * */
int t_drv_init(void)
{
int res = 0;
dev_t dev;

	res = alloc_chrdev_region(&dev, tchardev.minor,
							tchardev.count, tchardev.name);

	tchardev.major = MAJOR(dev);

	if (res < 0) {
		printk(KERN_WARNING "Can't allocate %s\n", MODULE_NAME);
		return res;
	} else {
		printk(KERN_WARNING "Allocated %s - major=%d and minor=%d",
               MODULE_NAME, tchardev.major, tchardev.minor);
	}
	cdev_setup(&(tchardev.cdev));

	return 0;
}


/***************************************************************************//**
 * @brief Device driver cleanup.
 *
 * */
void t_drv_exit(void)
{
dev_t devno = MKDEV(tchardev.major, tchardev.minor);

	if(tchardev.priv.count > 0){
		kfree(tchardev.priv.buf);
	}
	cdev_del(&(tchardev.cdev));
	unregister_chrdev_region(devno, tchardev.count);

	printk(KERN_INFO "%s correctly removed\n", MODULE_NAME);
}

module_init(t_drv_init);
module_exit(t_drv_exit);
