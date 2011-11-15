#ifndef __H_nxt_sense_core_h_
#define __H_nxt_sense_core_h_

struct nxt_sense_device_data {
  dev_t devt;
  struct cdev cdev;
  struct device *device;
  int (*get_sample)(int *);
};

extern int nxt_setup_sensor_chrdev(const struct file_operations *, struct nxt_sense_device_data *, const char *);
extern int nxt_teardown_sensor_chrdev(struct nxt_sense_device_data *);

#endif
