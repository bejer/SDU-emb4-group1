#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/types.h> // dev_t
#include <linux/fs.h> // file_operations (char driver methods)
#include <linux/kernel.h>
#include <linux/module.h> // THIS_MODULE
#include <linux/device.h> // class_create, device_create (and probably destroy)
#include <asm/uaccess.h> // copy_{from,to}_user
#include <linux/string.h>
#include <linux/slab.h>

#define DEVICE_NAME "box"
#define BOX_SIZE 256

//#define NUMBER_OF_DEVICES 5

static int number_of_devices = 1;
//MODULE_PARAM(nd, number_of_devices);
module_param(number_of_devices, int, 0);

struct io_status {
  bool bytes_outputted;
};

struct box_dev {
  //  dev_t devt; // Device major and minor numbers
  struct cdev cdev; // kernel char device abstraction
  //struct class *class; 
  char data[BOX_SIZE];
  struct io_status io_status;
};

static dev_t devt;
static struct class *class;

//static struct box_dev box_dev[number_of_devices];
struct box_dev **box_dev;

//static struct box_dev box_dev;

/* File operations */
ssize_t box_read(struct file *file, char *buf, size_t count, loff_t *ppos) {
  struct box_dev *bd = file->private_data;
  ssize_t bytes_read = strnlen(bd->data, BOX_SIZE);

  if (bd->io_status.bytes_outputted) {
    bd->io_status.bytes_outputted = false;
    return 0; // Indicate EOF
  } else {
    bd->io_status.bytes_outputted = true;
    if (copy_to_user(buf, bd->data, bytes_read) != 0) {
      return -EIO;
    }

    return bytes_read;
  }
}

ssize_t box_write(struct file *file, const char *buf, size_t count, loff_t *ppos) {
  struct box_dev *bd = file->private_data;
  if (copy_from_user(bd->data, buf, BOX_SIZE) != 0) {
    return -EIO;
  }

  return strnlen(bd->data, BOX_SIZE);
}

int box_open(struct inode *inode, struct file *file)
{
   int num = MINOR(inode->i_rdev);

   file->private_data = &box_dev[num];

   return 0;
}

static const struct file_operations box_fops = {
  .owner = THIS_MODULE,
  .read = box_read,
  .write = box_write,
  .open = box_open,
};

int __init box_init(void) {
  int i;

  //  box_dev = (struct box_dev *) kmalloc(number_of_devices * sizeof(struct box_dev), GFP_KERNEL);
  box_dev = kmalloc(number_of_devices * sizeof(struct box_dev), GFP_KERNEL);
  //  box_dev = kmalloc(256, GFP_KERNEL);

  /* Request dynamic allocation of a device major number */
  if (alloc_chrdev_region(&devt, 0, number_of_devices, DEVICE_NAME) < 0) {
    printk(KERN_DEBUG "Can't register device\n");
    return -1;
  }

  /* Populate sysfs entries */
  class = class_create(THIS_MODULE, DEVICE_NAME);
  if (IS_ERR(class)) {
    printk("Bad class create\n");
  }

  for (i = 0; i < number_of_devices; ++i) {
    /* Connect the file operations with the cdev */
    cdev_init(&box_dev[i]->cdev, &box_fops);
    box_dev[i]->cdev.owner = THIS_MODULE;

  /* Connect the major/minor number to the cdev */
    if (cdev_add(&box_dev[i]->cdev, devt + i, 1)) {
      printk("Bad cdev add for box number %d\n", i);
      return 1;
    }



    if (!device_create(class, NULL, MKDEV(MAJOR(devt), i), NULL, "box%d", i)) {
      printk(KERN_DEBUG "Got an error for device_create()\n");
      return -1;
    }

    strncpy(box_dev[i]->data, "Initial data...!\n", BOX_SIZE);
  }


  printk("Box driver initialised.\n");

  return 0;
}

void __exit box_exit(void) {
  int i;

  /* Release the major number */
  unregister_chrdev_region(devt, 1);

  for (i = 0; i < number_of_devices; ++i) {
    device_destroy(class, MKDEV(MAJOR(devt), i));
    cdev_del(&box_dev[i]->cdev);
  }

  class_destroy(class);

  kfree(box_dev);
}

module_init(box_init);
module_exit(box_exit);

MODULE_LICENSE("GPL");
