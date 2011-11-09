#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#include "touch.h"

#include "nxt_sense_core.h"

//#include "nxt_sense.h"

struct touch_data {
  int port;
};

static struct touch_data touch_data[4];

static int touch_open(struct inode *inode, struct file *filp) {
  filp->private_data = &touch_data[MINOR(inode->i_rdev)];

  return 0;
}

static ssize_t touch_read(struct file *filp, char __user *buff, size_t count, loff_t *offp) {
  size_t len;
  ssize_t status = 0;

  char output[6];
  int data = 0;
  int status_sampling;
  struct touch_data *td = filp->private_data;

  status_sampling = get_sample(td->port, &data);

  snprintf(output, 6, "%4.d\n", data);

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

static const struct file_operations touch_fops = {
  .owner = THIS_MODULE,
  .read = touch_read,
  .open = touch_open,
};

int add_touch_sensor(int port) {
  int res;
  printk("Inside init_touch_sensor\n");

  res = nxt_setup_sensor_chrdev(&touch_fops, port);

  printk("init_touch_sensor res: %d\n", res);

  touch_data[port].port = port;

  return res;
}

int remove_touch_sensor(int port) {
  int res;
  printk("Inside remove_touch_sensor\n");

  res = nxt_teardown_sensor_chrdev(port);

  touch_data[port].port = -1;

  return res;
}

MODULE_LICENSE("GPL");
