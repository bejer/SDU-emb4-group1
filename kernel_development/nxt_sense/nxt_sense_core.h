#ifndef __H_nxt_sense_core_h_
#define __H_nxt_sense_core_h_

extern int nxt_setup_sensor_chrdev(const struct file_operations *, const int);
extern int nxt_teardown_sensor_chrdev(const int);

#endif
