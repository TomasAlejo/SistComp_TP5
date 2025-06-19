#ifndef CDD_SIGNALS_H
#define CDD_SIGNALS_H

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/ioctl.h>

// Constantes
#define DEVICE_NAME "cdd_signals"
#define CLASS_NAME "cdd_class"
#define IOCTL_SELECT_SIGNAL _IOW('a', 1, int)

// Prototipos
static int cdd_open(struct inode *inode, struct file *file);
static int cdd_release(struct inode *inode, struct file *file);
static ssize_t cdd_read(struct file *file, char __user *buf, size_t len, loff_t *offset);
static long cdd_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static int __init cdd_init(void);
static void __exit cdd_exit(void);

#endif // CDD_SIGNALS_H
