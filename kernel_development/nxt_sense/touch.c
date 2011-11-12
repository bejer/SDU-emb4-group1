/* TODO:
 * Make sure the touch_data data is correctly (actually is) being uninitialised (pointer => NULL etc...)
 */

#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/kernel.h>
#include <linux/mutex.h>

#include "touch.h"

#include "nxt_sense_core.h"

//#include "nxt_sense.h"

#define DEVICE_NAME "touch"
#define DEFAULT_THRESHOLD 2048

struct touch_data {
  dev_t devt;
  struct cdev cdev;
  struct device *device;
  struct device_attribute dev_attr_threshold;
  struct device_attribute dev_attr_raw_sample;
  int (*get_sample)(int *);
  int port;
  int threshold;
  struct mutex mutex;
};

static struct touch_data touch_data[4];

bool obtain_nonblocking_lock(struct mutex *mutex) {
  if (mutex_trylock(mutex)) {
    return true;
  } else {
    return false;
  }
}

static int touch_open(struct inode *inode, struct file *filp) {
  struct touch_data *td;

  td = container_of(inode->i_cdev, struct touch_data, cdev);

  if (!mutex_trylock(&td->mutex)) {
    return -EBUSY;
  }

  filp->private_data = td;

  return 0;
}

static int touch_release(struct inode *inode, struct file *filp) {
  struct touch_data *td;
  td = container_of(inode->i_cdev, struct touch_data, cdev);

  mutex_unlock(&td->mutex);

  return 0;
}

/* The concurrency aware logic is in touch_open and touch_release */
static ssize_t touch_read(struct file *filp, char __user *buff, size_t count, loff_t *offp) {
  size_t len;
  ssize_t status = 0;

  char output[3];
  int data = 0;
  int status_sampling;
  struct touch_data *td = filp->private_data;

  //  status_sampling = get_sample(td->port, &data);
  status_sampling = td->get_sample(&data);

  if (data < td->threshold) {
    data = 1;
  } else {
    data = 0;
  }

  snprintf(output, 3, "%1.d\n", data);

  if (!buff)
    return -EFAULT;

  if (*offp > 0) {
    return 0;
  }

  len = strlen(output);

  if (len < count)
    count = len;

  if (copy_to_user(buff, output, count)) {
    return -EFAULT;
  } else {
    *offp += count;
    status = count;
  }

  return status;
}

static ssize_t threshold_show(struct device *dev, struct device_attribute *attr, char *buf) {
  int threshold;
  struct touch_data *td;
  td = container_of(attr, struct touch_data, dev_attr_threshold);

  if (!mutex_trylock(&td->mutex)) {
    return -EBUSY;
  }

  threshold = td->threshold;

  mutex_unlock(&td->mutex);

  return scnprintf(buf, PAGE_SIZE, "%d\n", threshold);
}

static ssize_t threshold_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) {
  int new_threshold = 0;
  int res = 0;
  struct touch_data *td;
  td = container_of(attr, struct touch_data, dev_attr_threshold);

  res = sscanf(buf, "%d", &new_threshold);

  if (res != 1) {
    printk(KERN_ALERT DEVICE_NAME "%d: wrong sysfs input for threshold, only takes a value for the threshold\n", MINOR(td->devt));
  } else {
    if (!mutex_trylock(&td->mutex)) {
      return -EBUSY;
    }
    
    td->threshold = new_threshold;
    
    mutex_unlock(&td->mutex);
  }

  return count;
}
/* See linux/stat.h for more info: S_IRUGO gives read permission for everyone and S_IWUSR gives write permission for the user (in this case root is the owner) */
//DEVICE_ATTR(threshold, (S_IRUGO | S_IWUSR), threshold_show, threshold_store);

/* NOTE: Should it have another error handling return -1 instead of printing -1? */
static ssize_t raw_sample_show(struct device *dev, struct device_attribute *attr, char *buf) {
  int status;
  int sample;
  struct touch_data *td;
  td = container_of(attr, struct touch_data, dev_attr_raw_sample);

  if (mutex_trylock(&td->mutex)) {
    return -EBUSY;
  }

  status = td->get_sample(&sample);

  mutex_unlock(&td->mutex);

  if (status != 0) {
    printk(KERN_ALERT DEVICE_NAME "%d: error trying to get a sample: %d\n", MINOR(td->devt), status);
    sample = -1;
  }

  return scnprintf(buf, PAGE_SIZE, "%d\n", sample);
}

//DEVICE_ATTR(raw_sample, (S_IRUGO), raw_sample_show, NULL);

static const struct file_operations touch_fops = {
  .owner = THIS_MODULE,
  .read = touch_read,
  .open = touch_open,
  .release = touch_release,
};

int init_sysfs(struct touch_data *td) {
  int error = 0;

/* Manually doing what the macro DEVICE_ATTR is doing behind the scenes, but working around it for having a structure for each touch_data */
  td->dev_attr_threshold.attr.name = __stringify(threshold);
  td->dev_attr_threshold.attr.mode = (S_IRUGO | S_IWUSR);
  td->dev_attr_threshold.show = threshold_show;
  td->dev_attr_threshold.store = threshold_store;

  td->dev_attr_raw_sample.attr.name = __stringify(raw_sample);
  td->dev_attr_raw_sample.attr.mode = (S_IRUGO);
  td->dev_attr_raw_sample.show = raw_sample_show;
  td->dev_attr_raw_sample.store = NULL;

  error = device_create_file(td->device, &td->dev_attr_threshold);
  if (error != 0) {
    printk(KERN_ALERT DEVICE_NAME "%d: device_create_file(threshold) error: %d\n", MINOR(td->devt), error);
    return -1;
  }

  error = device_create_file(td->device, &td->dev_attr_raw_sample);
  if (error != 0) {
    printk(KERN_ALERT DEVICE_NAME "%d: device_create_file(raw_sample) error: %d\n", MINOR(td->devt), error);
    device_remove_file(td->device, &td->dev_attr_threshold);
    return -1;
  }

  return 0;
}

int destroy_sysfs(struct touch_data *td) {
  device_remove_file(td->device, &td->dev_attr_threshold);
  device_remove_file(td->device, &td->dev_attr_raw_sample);

  return 0;
}

int add_touch_sensor(int port, dev_t devt) {
  int res;
  int error;
  printk("Inside init_touch_sensor\n");

  mutex_init(&touch_data[port].mutex);
  mutex_lock(&touch_data[port].mutex); /* void, so sleeps until the lock is acquired? mutex_lock_interruptible returns an error indicating it was interrupted... */
    

  touch_data[port].devt = devt;

  res = nxt_setup_sensor_chrdev(&touch_fops, &touch_data[port].cdev, &touch_data[port].devt, &touch_data[port].device, DEVICE_NAME, &touch_data[port].get_sample);

  printk("init_touch_sensor res: %d\n", res);

  touch_data[port].port = port;
  touch_data[port].threshold = DEFAULT_THRESHOLD;

  error = init_sysfs(&touch_data[port]);
  mutex_unlock(&touch_data[port].mutex);
  if (error != 0) {
    /* error .... handle this correct */
    return -1;
  }

  return res;
}

int uninitialise_touch_data(struct touch_data *td) {
  mutex_destroy(&td->mutex);

  return 0;
}

int remove_touch_sensor(int port) {
  int res;
  printk("Inside remove_touch_sensor\n");

  res = nxt_teardown_sensor_chrdev(&touch_data[port].cdev, &touch_data[port].devt);

  destroy_sysfs(&touch_data[port]);

  uninitialise_touch_data(&touch_data[port]);

  return res;
}

MODULE_LICENSE("GPL");
