#include "cdd_sim.h"

#define DEVICE_NAME "cdd_signals"
#define CLASS_NAME "cdd_class"
#define IOCTL_SELECT_SIGNAL _IOW('a', 1, int)

static dev_t dev_num;
static struct class* cdd_class;
static struct cdev cdd_cdev;

static int current_signal = 0;  // 0 = senoidal, 1 = triangular
static int t = 0;

// Señal senoidal simulada con 20 valores
static int sine_values[20] = {
     0, 16, 30, 41, 47, 50, 47, 41, 30, 16,
     0, -16, -30, -41, -47, -50, -47, -41, -30, -16
};

static int cdd_open(struct inode *inode, struct file *file) {
    return 0;
}

static int cdd_release(struct inode *inode, struct file *file) {
    return 0;
}

static ssize_t cdd_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
    int val;

    if (current_signal == 0) {
        val = sine_values[t % 20];
    } else {
        val = (t % 2 == 0) ? 100 : 0;
    }

    t++;

    if (copy_to_user(buf, &val, sizeof(int)))
        return -EFAULT;

    return sizeof(int);
}

static long cdd_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    switch (cmd) {
        case IOCTL_SELECT_SIGNAL:
            if (arg == 0 || arg == 1) {
                current_signal = arg;
                t = 0;
                printk(KERN_INFO "CDD: señal cambiada a %d\n", current_signal);
            } else {
                return -EINVAL;
            }
            break;
        default:
            return -EINVAL;
    }
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = cdd_open,
    .release = cdd_release,
    .read = cdd_read,
    .unlocked_ioctl = cdd_ioctl,
};

static int __init cdd_init(void) {
    alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    cdev_init(&cdd_cdev, &fops);
    cdev_add(&cdd_cdev, dev_num, 1);
    cdd_class = class_create(CLASS_NAME);  // Cambiado
    device_create(cdd_class, NULL, dev_num, NULL, DEVICE_NAME);
    printk(KERN_INFO "CDD señales cargado en /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit cdd_exit(void) {
    device_destroy(cdd_class, dev_num);
    class_destroy(cdd_class);
    cdev_del(&cdd_cdev);
    unregister_chrdev_region(dev_num, 1);
    printk(KERN_INFO "CDD señales descargado\n");
}

module_init(cdd_init);
module_exit(cdd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("GrupoNotFound");
MODULE_DESCRIPTION("Simulación de señales con driver");
