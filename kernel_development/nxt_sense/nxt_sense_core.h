#ifndef __H_nxt_sense_core_h_
#define __H_nxt_sense_core_h_

#if 0
extern int nxt_setup_sensor_chrdev(const struct file_operations *, struct cdev *, dev_t *, struct device **, const char *, int (**)(int *));
extern int nxt_teardown_sensor_chrdev(struct cdev *, dev_t *);
#endif

struct nxt_sense_device_data {
  dev_t devt;
  struct cdev cdev;
  struct device *device;
  int (*get_sample)(int *);
};

extern int nxt_setup_sensor_chrdev(const struct file_operations *, struct nxt_sense_device_data *, const char *);
extern int nxt_teardown_sensor_chrdev(struct nxt_sense_device_data *);

#endif
