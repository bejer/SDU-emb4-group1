#include <linux/module.h>
#include <linux/fs.h>

#include "touch.h"

//#include "nxt_sense.h"

int add_touch_sensor(int port) {
  printk("Inside init_touch_sensor\n");

  return 0;
}

int remove_touch_sensor(int port) {
  printk("Inside remove_touch-sensor\n");

  return 0;
}

MODULE_LICENSE("GPL");
