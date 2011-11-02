#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/spi/spi.h>
#include <linux/string.h>
#include <asm/uaccess.h>

#include "../adc/adc.h"

#define DEVICE_NAME "nxt_sense"

#define NUMBER_OF_DEVICES 1

struct nxt_sense_dev {
  dev_t devt;
  struct cdev cdev;
  struct class *class;
  //	char user_buff[USER_BUFF_SIZE];
  struct device *device;
};

static struct nxt_sense_dev nxt_sense_dev;

#if 0
static ssize_t nxt_sense_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
  size_t len;
  ssize_t status = 0;

  int sample_value;
  int adc_sample_status;

  if (!buff) 
    return -EFAULT;

  if (*offp > 0){
    printk(KERN_DEBUG "offp: %lld",*offp); 
    return 0;
  }
	
  adc_sample_status = adc_sample_channel(1, &sample_value);

  sprintf(nxt_sense_dev.user_buff, "ADC status: %d\nValue: %d\n", adc_sample_status, sample_value);

  len = strlen(nxt_sense_dev.user_buff);
 
  if (len < count) 
    count = len;

  if (copy_to_user(buff, nxt_sense_dev.user_buff, count))  {
    printk(KERN_DEBUG "nxt_sense_read(): copy_to_user() failed\n");
    status = -EFAULT;
  } else {
    *offp += count;
    status = count;
  }

  return status;	
}
#endif
static const struct file_operations nxt_sense_fops = {
  .owner =	THIS_MODULE,
};

static int __init nxt_sense_init_cdev(void)
{
  int error;

  nxt_sense_dev.devt = MKDEV(0, 0);

  error = alloc_chrdev_region(&nxt_sense_dev.devt, 0, NUMBER_OF_DEVICES, DEVICE_NAME);
  if (error < 0) {
    printk(KERN_ALERT "alloc_chrdev_region() in nxt_sense failed: %d \n", 
	   error);
    return -1;
  }

  cdev_init(&nxt_sense_dev.cdev, &nxt_sense_fops);
  nxt_sense_dev.cdev.owner = THIS_MODULE;

  error = cdev_add(&nxt_sense_dev.cdev, nxt_sense_dev.devt, 1);
  if (error) {
    printk(KERN_ALERT "cdev_add() failed: %d\n", error);
    unregister_chrdev_region(nxt_sense_dev.devt, 1);
    return -1;
  }	

  return 0;
}

static int __init nxt_sense_init_class(void)
{
  nxt_sense_dev.class = class_create(THIS_MODULE, DEVICE_NAME);

  if (IS_ERR(nxt_sense_dev.class)) {
    printk(KERN_ALERT "class_create() failed\n");
    return -1;
  }

  nxt_sense_dev.device = device_create(nxt_sense_dev.class, NULL, nxt_sense_dev.devt, NULL, DEVICE_NAME);

  if (IS_ERR(nxt_sense_dev.device)) {
    printk(KERN_ALERT "device_create(..., %s) failed: %ld\n", DEVICE_NAME, PTR_ERR(nxt_sense_dev.device));
    class_destroy(nxt_sense_dev.class);
    return -1;
  }

  return 0;
}

static int __init nxt_sense_init(void)
{
  memset(&nxt_sense_dev, 0, sizeof(nxt_sense_dev));

  if (nxt_sense_init_cdev() < 0) 
    goto fail_1;

  if (nxt_sense_init_class() < 0)  
    goto fail_2;

  return 0;

 fail_2:
  cdev_del(&nxt_sense_dev.cdev);
  unregister_chrdev_region(nxt_sense_dev.devt, 1);

 fail_1:
  return -1;
}
module_init(nxt_sense_init);

static void __exit nxt_sense_exit(void)
{
  device_destroy(nxt_sense_dev.class, nxt_sense_dev.devt);
  class_destroy(nxt_sense_dev.class);

  cdev_del(&nxt_sense_dev.cdev);
  unregister_chrdev_region(nxt_sense_dev.devt, 1);
}
module_exit(nxt_sense_exit);
MODULE_AUTHOR("Group1");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
