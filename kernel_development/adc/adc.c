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
#include <mach/gpio.h>

#define USER_BUFF_SIZE 128

#define SPI_BUS 1
#define SPI_BUS_CS0 0
#define SPI_BUS_SPEED 3000000
#define SPI_BITS_PER_WORD 8
#define DEVICE_NAME "gumnxtadc"

// Should be 8
#define NO_ADC_CHANNELS 5

//Sensor1 ADC IN0
#define ADC_CHANNEL0 0x00
#define ADC_CHANNEL1 0x08
#define ADC_CHANNEL2 0x10
#define ADC_CHANNEL3 0x18
//Voltage devider
#define ADC_CHANNEL4 0x20
// Currently no specified use on channel 5-7
#define ADC_CHANNEL5 0x28
#define ADC_CHANNEL6 0x30
#define ADC_CHANNEL7 0x38

#define SPI_BUFF_SIZE 4
// BUF size org. 2

#define GPIO_1OE 10
#define GPIO_2OE 71

#define number_of_devices 1

struct adc_dev {
  dev_t devt;
  struct cdev cdev;
  struct class *class;
  struct spi_device *spi_device;
  char user_buff[USER_BUFF_SIZE];
  struct device *device[NO_ADC_CHANNELS];
};

struct spi_control {
  struct spi_message msg;
  struct spi_transfer transfer;
  u8 *tx_buff; 
  u8 *rx_buff;
};

struct adc_info {
  int channel;
};

static struct spi_control spi_ctl;
static struct adc_dev adc_dev;
static struct adc_info adc_info;


static void spi_prepare_message(int channel)
{
  u8 adc_channel;
  spi_message_init(&spi_ctl.msg);

  switch(channel) {
  case 0:
    adc_channel = ADC_CHANNEL0;
    break;
  case 1:
    adc_channel = ADC_CHANNEL1;
    break;
  case 2:
    adc_channel = ADC_CHANNEL2;
    break;
  case 3:
    adc_channel = ADC_CHANNEL3;
    break;
  case 4:
    adc_channel = ADC_CHANNEL4;
    break;
  case 5:
    adc_channel = ADC_CHANNEL5;
    break;
  case 6:
    adc_channel = ADC_CHANNEL6;
    break;
  case 7:
    adc_channel = ADC_CHANNEL7;
    break;
  default:
    adc_channel = ADC_CHANNEL0;
    // signal some sort of error?
  }

  spi_ctl.tx_buff[0] = adc_channel;
  spi_ctl.tx_buff[1] = 0x00;

  /* Not doing message merging for now */
  spi_ctl.tx_buff[2] = 0x00;
  spi_ctl.tx_buff[3] = 0x00;
	
  memset(spi_ctl.rx_buff, 0, SPI_BUFF_SIZE);
  
  spi_ctl.transfer.tx_buf = spi_ctl.tx_buff;
  spi_ctl.transfer.rx_buf = spi_ctl.rx_buff;
  spi_ctl.transfer.len = SPI_BUFF_SIZE;
  
  spi_message_add_tail(&spi_ctl.transfer, &spi_ctl.msg);
}

static int spi_do_message(int channel)
{
  int status;

  spi_prepare_message(channel);

  /* sync'ed spi communication for now */
  status = spi_sync(adc_dev.spi_device, &spi_ctl.msg);

  return status;
}

static ssize_t adc_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
  size_t len;
  ssize_t status = 0;
  struct adc_info *adc_info = filp->private_data;
  int sample_value;

  if (!buff) 
    return -EFAULT;

  if (*offp > 0){
    printk(KERN_DEBUG "offp: %lld",*offp); 
    return 0;
  }
  if (!adc_dev.spi_device)
    strcpy(adc_dev.user_buff, "spi_device is NULL\n");
  else if (!adc_dev.spi_device->master)
    strcpy(adc_dev.user_buff, "spi_device->master is NULL\n");
  else {
    status = spi_do_message(adc_info->channel);

    sample_value = spi_ctl.rx_buff[2];
    sample_value = sample_value << 8;
    sample_value |= spi_ctl.rx_buff[3];
    sprintf(adc_dev.user_buff, "%X", sample_value);
    /*		sprintf(nxtts_dev.user_buff, "Status: %d\nADC0: TX: %.2X %.2X %.2X %.2X   RX: %.2X %.2X %.2X %.2X\nADC1: TX: %.2X %.2X %.2X %.2X   RX: %.2X %.2X %.2X %.2X\nADC2: TX: %.2X %.2X %.2X %.2X   RX: %.2X %.2X %.2X %.2X\nADC3: TX: %.2X %.2X %.2X %.2X   RX: %.2X %.2X %.2X %.2X\nADC4: TX: %.2X %.2X %.2X %.2X   RX: %.2X %.2X %.2X %.2X\n", nxtts_ctl.msg.status, 
		nxtts_ctl.tx_buff[0], nxtts_ctl.tx_buff[1],nxtts_ctl.tx_buff[2],nxtts_ctl.tx_buff[3], 
		nxtts_ctl.rx_buff[0], nxtts_ctl.rx_buff[1],nxtts_ctl.rx_buff[2],nxtts_ctl.rx_buff[3],
		nxtts_ctl.tx_buff[4], nxtts_ctl.tx_buff[5],nxtts_ctl.tx_buff[6],nxtts_ctl.tx_buff[7], 
		nxtts_ctl.rx_buff[4], nxtts_ctl.rx_buff[5],nxtts_ctl.rx_buff[6],nxtts_ctl.rx_buff[7],
		nxtts_ctl.tx_buff[8], nxtts_ctl.tx_buff[9],nxtts_ctl.tx_buff[10],nxtts_ctl.tx_buff[11], 
		nxtts_ctl.rx_buff[8], nxtts_ctl.rx_buff[9],nxtts_ctl.rx_buff[10],nxtts_ctl.rx_buff[11],
		nxtts_ctl.tx_buff[12], nxtts_ctl.tx_buff[13],nxtts_ctl.tx_buff[14],nxtts_ctl.tx_buff[15], 
		nxtts_ctl.rx_buff[12], nxtts_ctl.rx_buff[13],nxtts_ctl.rx_buff[14],nxtts_ctl.rx_buff[15],
		nxtts_ctl.tx_buff[16], nxtts_ctl.tx_buff[17],nxtts_ctl.tx_buff[18],nxtts_ctl.tx_buff[19],
		nxtts_ctl.rx_buff[16], nxtts_ctl.rx_buff[17],nxtts_ctl.rx_buff[18],nxtts_ctl.rx_buff[19]);
    */
  }

  len = strlen(adc_dev.user_buff);
 
  if (len < count) 
    count = len;

  if (copy_to_user(buff, adc_dev.user_buff, count))  {
    printk(KERN_DEBUG "adc_read(): copy_to_user() failed\n");
    status = -EFAULT;
  } else {
    *offp += count;
    status = count;
  }

  return status;	
}

static int adc_open(struct inode *inode, struct file *filp)
{	
  //	int status = 0;

  adc_info.channel = MINOR(inode->i_rdev);
  filp->private_data = &adc_info;

  return 0;
}

static int adc_probe(struct spi_device *spi_device)
{
  adc_dev.spi_device = spi_device;

  return 0;
}

static int adc_remove(struct spi_device *spi_device)
{
  adc_dev.spi_device = NULL;

  return 0;
}

static int __init add_adc_device_to_bus(void)
{
  struct spi_master *spi_master;
  struct spi_device *spi_device;
  struct device *pdev;
  char buff[64];
  int status = 0;

  /* This call returns a refcounted pointer to the relevant spi_master - the caller must release this pointer(device_put()) */	
  spi_master = spi_busnum_to_master(SPI_BUS);
  if (!spi_master) {
    printk(KERN_ALERT "spi_busnum_to_master(%d) returned NULL\n", SPI_BUS);
    printk(KERN_ALERT "Missing modprobe omap2_mcspi?\n");
    return -1;
  }

  spi_device = spi_alloc_device(spi_master);
  if (!spi_device) {
    printk(KERN_ALERT "spi_alloc_device() failed\n");
    return -1;
  }

  spi_device->chip_select = SPI_BUS_CS0;

  /* Check whether this SPI bus.cs is already claimed */
  /* snprintf the c-way of formatting a string */
  snprintf(buff, sizeof(buff), "%s.%u", dev_name(&spi_device->master->dev), spi_device->chip_select);

  pdev = bus_find_device_by_name(spi_device->dev.bus, NULL, buff);
  if (pdev) {
    /* We are not going to use this spi_device, so free it. Since spi_device is not added then decrement the refcount */ 
    spi_dev_put(spi_device);

    /* 
     * There is already a device configured for this bus.cs  
     * It is okay if it us, otherwise complain and fail.
     */
    if (pdev->driver && pdev->driver->name && strcmp(DEVICE_NAME, pdev->driver->name)) {
      printk(KERN_ALERT "Driver [%s] already registered for %s\n", pdev->driver->name, buff);
      status = -1;
    } 
  } else {
    spi_device->max_speed_hz = SPI_BUS_SPEED;
    spi_device->mode = SPI_MODE_0;
    spi_device->bits_per_word = SPI_BITS_PER_WORD;
    spi_device->irq = -1;
    spi_device->controller_state = NULL;
    spi_device->controller_data = NULL;
    strlcpy(spi_device->modalias, DEVICE_NAME, SPI_NAME_SIZE);

    status = spi_add_device(spi_device);		
    if (status < 0) {
      /* If spi_device is not added then decrement the refcount */	
      spi_dev_put(spi_device);
      printk(KERN_ALERT "spi_add_device() failed: %d\n", status);		
    }				
  }
  /* See comment for spi_busnum_to_master */
  put_device(&spi_master->dev);

  return status;
}

static struct spi_driver spi_driver = {
  .driver = {
    .name =	DEVICE_NAME,
    .owner = THIS_MODULE,
  },
  .probe = adc_probe,
  .remove = __devexit_p(adc_remove),	
};

static int __init init_spi(void)
{
  int error;

  spi_ctl.tx_buff = kzalloc(SPI_BUFF_SIZE, GFP_KERNEL | GFP_DMA);
  if (!spi_ctl.tx_buff) {
    error = -ENOMEM;
    goto init_error;
  }

  spi_ctl.rx_buff = kzalloc(SPI_BUFF_SIZE, GFP_KERNEL | GFP_DMA);
  if (!spi_ctl.rx_buff) {
    error = -ENOMEM;
    goto init_error;
  }

  error = spi_register_driver(&spi_driver);
  if (error < 0) {
    printk(KERN_ALERT "spi_register_driver() failed %d\n", error);
    return error;
  }

  error = add_adc_device_to_bus();
  if (error < 0) {
    printk(KERN_ALERT "add_adc_to_bus() failed\n");
    spi_unregister_driver(&spi_driver);
    return error;
  }

  return 0;

 init_error:

  if (spi_ctl.tx_buff) {
    kfree(spi_ctl.tx_buff);
    spi_ctl.tx_buff = 0;
  }

  if (spi_ctl.rx_buff) {
    kfree(spi_ctl.rx_buff);
    spi_ctl.rx_buff = 0;
  }

  return error;
}

static const struct file_operations adc_fops = {
  .owner =	THIS_MODULE,
  .read = 	adc_read,
  .open =		adc_open,	
};

static int __init init_cdev(void)
{
  int error;

  adc_dev.devt = MKDEV(0, 0);

  error = alloc_chrdev_region(&adc_dev.devt, 0, NO_ADC_CHANNELS, DEVICE_NAME);
  if (error < 0) {
    printk(KERN_ALERT "alloc_chrdev_region() failed: %d \n", 
	   error);
    return -1;
  }

  cdev_init(&adc_dev.cdev, &adc_fops);
  adc_dev.cdev.owner = THIS_MODULE;

  error = cdev_add(&adc_dev.cdev, adc_dev.devt, 1);
  if (error) {
    printk(KERN_ALERT "cdev_add() failed: %d\n", error);
    unregister_chrdev_region(adc_dev.devt, 1);
    return -1;
  }	

  return 0;
}

static int __init init_class(void)
{
  int j;
  int i;

  adc_dev.class = class_create(THIS_MODULE, DEVICE_NAME);

  if (IS_ERR(adc_dev.class)) {
    printk(KERN_ALERT "class_create(%s) failed: %ld\n", DEVICE_NAME, PTR_ERR(adc_dev.class));
    return -1;
  }

  for (i = 0; i < NO_ADC_CHANNELS; ++i) {
    adc_dev.device[i] = device_create(adc_dev.class, NULL, MKDEV(MAJOR(adc_dev.devt), i), NULL, "gumnxtadc%d", i);

    if (IS_ERR(adc_dev.device)) {
      printk(KERN_ALERT "device_create(..., %s) failed: %ld\n", DEVICE_NAME, PTR_ERR(adc_dev.device));
      goto failed_device_creation;
    }
  }

  return 0;

 failed_device_creation:
  for (j = i-1; j >= 0; --j) {
    device_destroy(adc_dev.class, MKDEV(MAJOR(adc_dev.devt), j));
  }

  class_destroy(adc_dev.class);
  return -1;
}

static int __init init_level_shifters(void) {

  if (gpio_request(GPIO_1OE, "SPI1OE")) {
    printk(KERN_ALERT "gpio_request failed for SPI1OE\n");
    goto init_pins_fail_1;
  }

  if (gpio_request(GPIO_2OE, "SPI2OE")) {
    printk(KERN_ALERT "gpio_request failed for SPI2OE\n");
    goto init_pins_fail_2;
  }

  if (gpio_direction_output(GPIO_1OE, 0)) {
    printk(KERN_ALERT "gpio_direction_output GPIO_1OE failed\n");
    goto init_pins_fail_3;
  }

  if (gpio_direction_output(GPIO_2OE, 0)) {
    printk(KERN_ALERT "gpio_direction_output GPIO_2OE failed\n");
    goto init_pins_fail_3;
  }

  //  goto init_pins_fail_1;
  return 0;

 init_pins_fail_3:
  gpio_free(GPIO_2OE);

 init_pins_fail_2:
  gpio_free(GPIO_1OE);

 init_pins_fail_1:

  return -1;
}

static int __init adc_init(void)
{
  int j;

  memset(&adc_dev, 0, sizeof(adc_dev));
  memset(&spi_ctl, 0, sizeof(spi_ctl));

  if (init_cdev() < 0) 
    goto fail_1;

  if (init_class() < 0)  
    goto fail_2;

  if (init_spi() < 0) 
    goto fail_3;

  if (init_level_shifters() < 0)
    goto fail_4;


  return 0;

 fail_4:
  gpio_free(GPIO_2OE);
  gpio_free(GPIO_1OE);

 fail_3:
  for (j = NO_ADC_CHANNELS; j >= 0; --j) {
    device_destroy(adc_dev.class, MKDEV(MAJOR(adc_dev.devt), j));
  }
  class_destroy(adc_dev.class);

 fail_2:
  cdev_del(&adc_dev.cdev);
  unregister_chrdev_region(adc_dev.devt, 1);

 fail_1:
  return -1;
}
module_init(adc_init);

static void __exit adc_exit(void)
{
  int j;

  spi_unregister_driver(&spi_driver);

  for (j = NO_ADC_CHANNELS; j >= 0; --j) {
    device_destroy(adc_dev.class, MKDEV(MAJOR(adc_dev.devt), j));
  }
  class_destroy(adc_dev.class);

  cdev_del(&adc_dev.cdev);
  unregister_chrdev_region(adc_dev.devt, 1);

  if (spi_ctl.tx_buff)
    kfree(spi_ctl.tx_buff);

  if (spi_ctl.rx_buff)
    kfree(spi_ctl.rx_buff);

  if (adc_dev.user_buff)
    kfree(adc_dev.user_buff);

  /* Release spi level shifter pins */
  gpio_free(GPIO_2OE);
  gpio_free(GPIO_1OE);

}
module_exit(adc_exit);
MODULE_AUTHOR("Group1");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
