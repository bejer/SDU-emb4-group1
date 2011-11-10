/* TODO:
 * Make the port numbers (0-3 or 1-4) work correctly with the minor number that is given to the nxt_sense char device!!! Future task: Make the port numbers able to run from an arbitrary number, such that it is not required to have minor numbers starting from 0.
 */

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

/* Better setup of the dependencies */
#include "../adc/adc.h"

/* All these submodules includes and parsers? could be set in a single h file for that purpose */
#include "touch.h"

#define DEVICE_NAME "nxt_sense"

#define NUMBER_OF_DEVICES 5

#define PORT_MIN 0
#define NUMBER_OF_PORTS 4
/* This requires the port_min to start from 0 unless the number_of_ports is incremented accordingly */
#define NXT_SENSE_MINOR 4

#define NONWORKING_PORT_CODE -1
#define NONE_CODE 0
#define TOUCH_CODE 1
#define LIGHT_CODE 2
#define MAX_SENSOR_CODE LIGHT_CODE

static const char *sensor_names[] = {NULL, "touch", "light"};

struct nxt_sense_dev {
  dev_t devt;
  struct cdev cdev;
  struct class *class;
  //	char user_buff[USER_BUFF_SIZE];
  struct device *device;
  int port_cfg[NUMBER_OF_PORTS];
};

struct nxt_sensor_dev {
  struct cdev cdev;
  struct device *device;
};

static struct nxt_sense_dev nxt_sense_dev;
static struct nxt_sensor_dev nxt_sensor_dev[NUMBER_OF_PORTS];

static bool valid_port(int port) {
  bool res = false;
  if (port >= PORT_MIN && port < (PORT_MIN + NUMBER_OF_PORTS))
    res = true;

  return res;
}

static char const *get_sensor_name(int port) {
  int sensor_code;

  if (!valid_port(port)) {
    return NULL;
  }

  sensor_code = nxt_sense_dev.port_cfg[port];

  if (sensor_code > NONE_CODE && sensor_code <= MAX_SENSOR_CODE) {
    return sensor_names[sensor_code];
  } else {
    printk(KERN_ALERT DEVICE_NAME ": There is no named sensor on port %d, sensor_code = %d\n", port, sensor_code);
    printk(KERN_ALERT DEVICE_NAME ": sensor_code [%d], [%d], [%d], [%d]\n", nxt_sense_dev.port_cfg[0], nxt_sense_dev.port_cfg[1], nxt_sense_dev.port_cfg[2], nxt_sense_dev.port_cfg[3]);
  }

  return NULL;
}
/* NOTE: should have a precondition/check of whether or not the sensor_code on the specified port is 0 (available) */
static int load_nxt_sensor(int sensor_code, int port) {
  int status = 0;

  nxt_sense_dev.port_cfg[port] = sensor_code;

  switch (sensor_code) {
  case NONE_CODE:
    /* Do nothing */
    break;
  case TOUCH_CODE:
    status = add_touch_sensor(port);
    break;
  case LIGHT_CODE:
    
    break;
  case NONWORKING_PORT_CODE:
    /* Should it output that there is a bug in the code? */
    printk("nxt_sense internal error: Trying to load a nxt_sensor with NONWORKING_PORT_CODE (%d)\n", sensor_code);
    status = -1;
    break;
  default:
    /* Error... */
    printk(KERN_ALERT "nxt_sense: Loading unknown sensor port code!: %d\n", sensor_code);
    status = -1;
  }

  if(status < 0){
    nxt_sense_dev.port_cfg[port] = NONWORKING_PORT_CODE; // Maybe just NONE_CODE since the adding tries to clean up for itself...
  }

  return status;
}

/* NOTE: Should have a precondition/check that takes care of handling cases where the sensor code is NONE_CODE (no devices allocated/created/assigned) */
static int unload_nxt_sensor(int sensor_code, int port) {
  int status = 0;

  switch (sensor_code) {
  case NONE_CODE:
    /* Do nothing */
    break;
  case TOUCH_CODE:
    status = remove_touch_sensor(port);
    break;
  case LIGHT_CODE:
    
    break;
  case NONWORKING_PORT_CODE:
    /* Should it output that there is a bug in the code? */
    printk("nxt_sense internal error: Trying to unload a nxt_sensor with NONWORKING_PORT_CODE (%d)\n", sensor_code);
    status = -1;
    break;
  default:
    /* Error... */
    printk(KERN_ALERT "nxt_sense: Unloading unknown sensor port code!: %d\n", sensor_code);
    status = -1;
  }

  if(status < 0){
    nxt_sense_dev.port_cfg[port] = NONWORKING_PORT_CODE;
  } else {
    nxt_sense_dev.port_cfg[port] = NONE_CODE;
  }

  return status;
}

/* Hook to be called from the sensor submodules */
/* NOTE: These functions sould not be called outside load and unload nxt_modules.... */
/* NOTE: Should the majority of this code be placed in the drivers? eventhough it will be the same for all drivers - how about giving them the responsibility of creating extra devices and destroying them together with sysfs entries */
/* NOTE: should give access to the nxt_sensor_dev[port].device pointer for creating sysfs entries */
int nxt_setup_sensor_chrdev(const struct file_operations *fops, const int port) {
  int error;

  if (!valid_port(port)) {
    return -1;
  }

  cdev_init(&nxt_sensor_dev[port].cdev, fops);
  
  error = cdev_add(&nxt_sensor_dev[port].cdev, MKDEV(MAJOR(nxt_sense_dev.devt), port), 1);
  if (error) {
    printk(KERN_ALERT DEVICE_NAME ": could not add cdev for %s: %d\n", get_sensor_name(port), error);
    return -1;
  }

  nxt_sensor_dev[port].device = device_create(nxt_sense_dev.class, nxt_sense_dev.device, MKDEV(MAJOR(nxt_sense_dev.devt), port), NULL, "%s%d", get_sensor_name(port), port);
  if (IS_ERR(nxt_sensor_dev[port].device)) {
    printk(KERN_ALERT DEVICE_NAME ": device_create() failed for sensor %s: %ld", DEVICE_NAME, PTR_ERR(nxt_sensor_dev[port].device));
    cdev_del(&nxt_sensor_dev[port].cdev);
    return -1;
  }

  return 0;
}

int nxt_teardown_sensor_chrdev(const int port) {
  if (!valid_port(port)) {
    return -1;
  }

  device_destroy(nxt_sense_dev.class, MKDEV(MAJOR(nxt_sense_dev.devt), port));
  cdev_del(&nxt_sensor_dev[port].cdev);

  return 0;
}

/* NOTE: Handle the channels accordingly to the port - lucky that it fits right now, but should be made more dynamically / robust */
int get_sample(const int port, int *data) {
  int status;

  if (!valid_port(port)) {
    return -1;
  }

  status = adc_sample_channel(port, data);

  if (status != 0) {
    printk(KERN_ALERT DEVICE_NAME ": Some error happened while communicating with the ADC: %d\n", status);
  }

  return status;
}

static int update_port_cfg(int cfg[]) {
  int res;
  int i;
  int error_occurred = 0;
  for (i = 0; i < NUMBER_OF_PORTS; ++i) {
    if (cfg[i] < NONE_CODE || cfg[i] > MAX_SENSOR_CODE) {
      return -1;
    }
  }

  for (i = 0; i < NUMBER_OF_PORTS; ++i) {
    if (nxt_sense_dev.port_cfg[i] != cfg[i] && nxt_sense_dev.port_cfg[i] != NONWORKING_PORT_CODE) {
      res = unload_nxt_sensor(nxt_sense_dev.port_cfg[i], i);
      if (res != 0) {
        error_occurred = -2;
	continue;
      }

      res = load_nxt_sensor(cfg[i], i);
      if (res != 0) {
	error_occurred = -3;
	continue;
      }

    }
  }


  return error_occurred;
}

static ssize_t nxt_sense_show(struct device *dev, struct device_attribute *attr, char *buf) {
  
  return scnprintf(buf, PAGE_SIZE, "%d %d %d %d\n", nxt_sense_dev.port_cfg[0], nxt_sense_dev.port_cfg[1], nxt_sense_dev.port_cfg[2], nxt_sense_dev.port_cfg[3]);
}

static ssize_t nxt_sense_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) {
  int p[NUMBER_OF_PORTS];
  int res;
  int status;

  res = sscanf(buf, "%d %d %d %d", &p[0], &p[1], &p[2], &p[3]);

  if (res != NUMBER_OF_PORTS) {
    printk("nxt_sense: sysfs input was not four integers!\n");
  } else {
    printk("nxt_sense: sysfs input: %d %d %d %d\n", p[0], p[1], p[2], p[3]);

    status = update_port_cfg(p);
    if (status != 0) {
      printk(KERN_ALERT DEVICE_NAME ": error from update_port_cfg([%d] [%d] [%d] [%d]): %d\n", p[0], p[1], p[2], p[3], status);
    }
  }

  return count;
}

DEVICE_ATTR(nxt_sense, 0600, nxt_sense_show, nxt_sense_store);

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

  error = cdev_add(&nxt_sense_dev.cdev, MKDEV(MAJOR(nxt_sense_dev.devt), NXT_SENSE_MINOR), 1);
  if (error) {
    printk(KERN_ALERT "cdev_add() failed: %d\n", error);
    unregister_chrdev_region(nxt_sense_dev.devt, NUMBER_OF_DEVICES);
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

  nxt_sense_dev.device = device_create(nxt_sense_dev.class, NULL, MKDEV(MAJOR(nxt_sense_dev.devt), NXT_SENSE_MINOR), NULL, DEVICE_NAME);

  if (IS_ERR(nxt_sense_dev.device)) {
    printk(KERN_ALERT "device_create(..., %s) failed: %ld\n", DEVICE_NAME, PTR_ERR(nxt_sense_dev.device));
    class_destroy(nxt_sense_dev.class);
    return -1;
  }

  if (device_create_file(nxt_sense_dev.device, &dev_attr_nxt_sense)) {
    printk("device_create_file error: -EXISTS (hardcoded)");
    device_destroy(nxt_sense_dev.class, MKDEV(MAJOR(nxt_sense_dev.devt), NXT_SENSE_MINOR));
    class_destroy(nxt_sense_dev.class);
    return -1;
  }

  return 0;
}

static int __init nxt_sense_init(void)
{
  printk("Inside nxt_sense_init\n");
  memset(&nxt_sense_dev, 0, sizeof(nxt_sense_dev));

  if (nxt_sense_init_cdev() < 0) 
    goto fail_1;

  if (nxt_sense_init_class() < 0)  
    goto fail_2;

  return 0;

 fail_2:
  cdev_del(&nxt_sense_dev.cdev);
  unregister_chrdev_region(nxt_sense_dev.devt, NUMBER_OF_DEVICES);

 fail_1:
  return -1;
}
module_init(nxt_sense_init);

static void __exit nxt_sense_exit(void)
{
  int p[NUMBER_OF_PORTS] = {0, 0, 0, 0};
  update_port_cfg(p);

  device_remove_file(nxt_sense_dev.device, &dev_attr_nxt_sense);
  device_destroy(nxt_sense_dev.class, MKDEV(MAJOR(nxt_sense_dev.devt), NXT_SENSE_MINOR));
  class_destroy(nxt_sense_dev.class);

  cdev_del(&nxt_sense_dev.cdev);
  unregister_chrdev_region(nxt_sense_dev.devt, NUMBER_OF_DEVICES);
}
module_exit(nxt_sense_exit);
MODULE_AUTHOR("Group1");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
