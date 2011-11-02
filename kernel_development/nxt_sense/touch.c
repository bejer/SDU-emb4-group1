#include <linux/module.h>
#include <linux/fs.h>

#include "../../nxt_sense.h"

static int add_touch_sensor(int port) {
  printk("Inside init_touch_sensor\n");
}

static int remove_touch_sensor(int port) {
  printk("Inside remove_touch-sensor\n");
}
