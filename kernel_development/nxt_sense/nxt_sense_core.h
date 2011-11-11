#ifndef __H_nxt_sense_core_h_
#define __H_nxt_sense_core_h_

extern int nxt_setup_sensor_chrdev(const struct file_operations *, struct cdev *, dev_t *, struct device **, const char *, int (**)(int *));
extern int nxt_teardown_sensor_chrdev(struct cdev *, dev_t *);

extern int get_sample(const int, int *);

#endif
