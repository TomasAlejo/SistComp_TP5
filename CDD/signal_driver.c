/**
 * @file signal_driver.c
 * @brief TP5 - Simulador de señales cuadrada y triangular (CDD)
 * @author Marcos Agustín Reyeros
 * @date Junio 2025
 *
 * Este módulo del kernel implementa un Character Device Driver que simula dos señales:
 * - Señal cuadrada: alterna entre 0 y 1 cada 0.5 segundos.
 * - Señal triangular: sube y baja entre 0 y 100 con incrementos de 2 cada 10ms.
 *
 * El usuario puede seleccionar la señal deseada escribiendo "1" o "2" al dispositivo,
 * y puede leer el valor actual con `cat /dev/signal_driver`.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

#define DEVICE_NAME "signal_driver"
#define CLASS_NAME "signal_class"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Marcos Agustín Reyeros");
MODULE_DESCRIPTION("Simulador de señales cuadrada y triangular");

static dev_t dev_num;
static struct cdev c_dev;
static struct class *cl;
static struct timer_list my_timer;

static int signal1 = 0;
static int signal2 = 0;
static int dir = 1;
static int ticks = 0;
static int selected_signal = 1;

static void timer_callback(struct timer_list *t)
{
    ticks++;
    if (ticks >= 50) {
        signal1 = !signal1;
        ticks = 0;
    }

    signal2 += 2 * dir;
    if (signal2 >= 100) {
        signal2 = 100;
        dir = -1;
    } else if (signal2 <= 0) {
        signal2 = 0;
        dir = 1;
    }

    mod_timer(&my_timer, jiffies + HZ / 100); // 10 ms
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    char msg[16];
    int size;
    int value = (selected_signal == 1) ? signal1 : signal2;

    if (*off > 0)
        return 0;

    size = snprintf(msg, sizeof(msg), "%d\n", value);
    if (copy_to_user(buf, msg, size))
        return -EFAULT;

    *off += size;
    return size;
}

static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
    char kbuf[2];
    if (len < 1 || copy_from_user(kbuf, buf, 1))
        return -EFAULT;

    if (kbuf[0] == '1')
        selected_signal = 1;
    else if (kbuf[0] == '2')
        selected_signal = 2;
    else
        return -EINVAL;

    return len;
}

static int my_open(struct inode *i, struct file *f) {
    printk(KERN_INFO "signal_driver: open()\n");
    return 0;
}

static int my_close(struct inode *i, struct file *f) {
    printk(KERN_INFO "signal_driver: close()\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
    .read = my_read,
    .write = my_write,
};

static int __init signal_init(void)
{
    int ret;
    struct device *dev_ret;

    if ((ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME)) < 0)
        return ret;

    if (IS_ERR(cl = class_create(CLASS_NAME))) {
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(cl);
    }

    if (IS_ERR(dev_ret = device_create(cl, NULL, dev_num, NULL, DEVICE_NAME))) {
        class_destroy(cl);
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(dev_ret);
    }

    cdev_init(&c_dev, &fops);
    if ((ret = cdev_add(&c_dev, dev_num, 1)) < 0) {
        device_destroy(cl, dev_num);
        class_destroy(cl);
        unregister_chrdev_region(dev_num, 1);
        return ret;
    }

    timer_setup(&my_timer, timer_callback, 0);
    mod_timer(&my_timer, jiffies + HZ / 100);
    printk(KERN_INFO "signal_driver: cargado correctamente\n");
    return 0;
}

static void __exit signal_exit(void)
{
    del_timer_sync(&my_timer);
    cdev_del(&c_dev);
    device_destroy(cl, dev_num);
    class_destroy(cl);
    unregister_chrdev_region(dev_num, 1);
    printk(KERN_INFO "signal_driver: descargado\n");
}

module_init(signal_init);
module_exit(signal_exit);
