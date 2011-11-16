/* TODO:
 * Make sure the "bad" scenarios are handled correctly (such as what to do in initialise_level_shifter if gpio_request fails - should it uninitialise itself and free the level_shifter, and what about error codes?)
 */
#include <linux/module.h>
#include <mach/gpio.h>
#include <linux/kref.h>
#include <linux/mutex.h>
#include <linux/slab.h>

#define DEVICE_NAME "level_shifter"

/* Only one level shifter (2OE on it) that is shared for now */
#define GPIO_2OE 71

DEFINE_MUTEX(ls_mutex);

struct level_shifter {
  struct kref refcount;
};

static struct level_shifter *level_shifter;

/***********************************************************************
 *
 * Utility functions for initialising and uninitialising the
 * level_shifter
 *
 ***********************************************************************/
/* What to do when gpio_request fails? (see TODO at the top) */
static void initialise_level_shifter(void) {
  if (gpio_request(GPIO_2OE, "LS2OE")) {
    printk(KERN_CRIT DEVICE_NAME ": gpio_request failed for LS2OE\n");
  } else {
    if (gpio_direction_output(GPIO_2OE, 0)) {
      printk(KERN_CRIT DEVICE_NAME ": gpio_direction_output failed for LS2OE\n");
    }
  }

  kref_init(&level_shifter->refcount);
}

static void uninitialise_level_shifter(struct kref *kref) {
  gpio_free(GPIO_2OE);

  kfree(level_shifter);
  level_shifter = NULL;
}

/***********************************************************************
 *
 * Hooks for registering and unregistering the use of the GPIO pin
 *
 ***********************************************************************/
/* Returns 0 on success, else a negative error code (not implemented yet) */
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
    kref_get(&level_shifter->refcount);
  }

  mutex_unlock(&ls_mutex);

  return 0;  
}

static int unregister_use_of_level_shifter(void) {
  mutex_lock(&ls_mutex);
  if (!level_shifter) {
    printk(KERN_WARNING DEVICE_NAME ": trying to unregister without a register (or someone has unregistered twice), anyhow level_shifter is NULL!\n");
    return -1;
  }
  kref_put(&level_shifter->refcount, uninitialise_level_shifter);

  mutex_unlock(&ls_mutex);

  return 0;
}

EXPORT_SYMBOL(register_use_of_level_shifter);
EXPORT_SYMBOL(unregister_use_of_level_shifter);

/***********************************************************************
 *
 * Module exit function to make sure the module cleans up after itself
 *
 ***********************************************************************/
static void __exit level_shifter_exit(void) {
  while (level_shifter) {
    uninitialise_level_shifter(NULL);
  }
}

module_exit(level_shifter_exit);

MODULE_LICENSE("GPL");
