#include <linux/module.h>
#include <mach/gpio.h>
#include <linux/kref.h>
#include <linux/mutex.h>
#include <linux/slab.h>

#define DEVICE_NAME "level_shifter"

/* Only 2OE that is shared? */
//#define GPIO_1OE 10
#define GPIO_2OE 71

/* Should this be encapsulated behind a static defined mutex? to avoid kzallocing twice... */
DEFINE_MUTEX(ls_mutex);

struct level_shifter {
  struct kref refcount;
};

static struct level_shifter *level_shifter;

/* What to do when gpio_request fails? call uninitialise_level_shifter(NULL) ? */
static void initialise_level_shifter(void) {
#if 1

  printk(DEVICE_NAME ": inside initialise_level_shifter\n");


#endif
#if 0
  if (gpio_request(GPIO_2OE, "LS2OE")) {
    printk(KERN_ALERT DEVICE_NAME ": gpio_request failed for LS2OE\n");
  } else {
    if (gpio_direction_output(GPIO_2OE, 0)) {
      printk(KERN_ALERT DEVICE_NAME ": gpio_direction_output failed for LS2OE\n");
    }
  }
#endif

  kref_init(&level_shifter->refcount);
}

static void uninitialise_level_shifter(struct kref *kref) {
#if 1

  printk(DEVICE_NAME ": inside uninitialise_level_shifter\n");

#endif

#if 0
  gpio_free(GPIO_2OE);
#endif
  kfree(level_shifter);
  level_shifter = NULL;
}

static int register_use_of_level_shifter(void) {
  mutex_lock(&ls_mutex);

  if (!level_shifter) {
    level_shifter = kmalloc(sizeof(struct level_shifter), GFP_KERNEL);
    if (!level_shifter) {
      mutex_unlock(&ls_mutex);
      return -ENOMEM;
    }
    initialise_level_shifter();
  } else {
#if 1
    printk(DEVICE_NAME ": refcount before register: %d\n", level_shifter->refcount.refcount.counter);
#endif
    kref_get(&level_shifter->refcount);
#if 1
    printk(DEVICE_NAME ": refcount after register: %d\n", level_shifter->refcount.refcount.counter);
#endif
  }

  mutex_unlock(&ls_mutex);

  return 0;  
}

static int unregister_use_of_level_shifter(void) {
  mutex_lock(&ls_mutex);
  if (!level_shifter) {
    printk(KERN_ALERT DEVICE_NAME ": trying to unregister without a register (or someone has unregistered twice), anyhow level_shifter is NULL\n");
    return -1;
  }
#if 1
    printk(DEVICE_NAME ": refcount before unregister: %d\n", level_shifter->refcount.refcount.counter);
#endif
  kref_put(&level_shifter->refcount, uninitialise_level_shifter);
#if 1
  if (!level_shifter) {
    printk(DEVICE_NAME ": refcount after unregister: level_shifter is null - fully dereferenced\n");
  } else {
    printk(DEVICE_NAME ": refcount after unregister: %d\n", level_shifter->refcount.refcount.counter);
  }
#endif

  mutex_unlock(&ls_mutex);

  return 0;
}

EXPORT_SYMBOL(register_use_of_level_shifter);
EXPORT_SYMBOL(unregister_use_of_level_shifter);

MODULE_LICENSE("GPL");
