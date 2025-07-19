#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/device.h> 
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define BUFFER_SIZE 256
#define DRIVE_NAME "gpio"
#define CLASS_NAME "pi4"
#define MINOR_BASE 0
#define MINOR_COUNT 1

static int pi_open(struct inode *, struct file *);
static int pi_release(struct inode *, struct file *);
static ssize_t pi_read (struct file *, char __user *, size_t, loff_t *);
static ssize_t pi_write (struct file *, const char __user *, size_t, loff_t *);
int __init pi_driver_init(void);
void __exit pi_driver_exit(void);

char buff[BUFFER_SIZE];
static dev_t pi_dev_num;
static struct cdev pi_cdev;
static struct class *pi_class;
static struct device *pi_device;
static struct file_operations pi_fops = {
    .owner = THIS_MODULE,
    .open = pi_open,
    .release = pi_release,
    .read = pi_read, 
    .write = pi_write,
};

int __init pi_driver_init(){
    int err = alloc_chrdev_region(&pi_dev_num, MINOR_BASE, MINOR_COUNT, DRIVE_NAME);
    if (err != 0){
        printk(KERN_WARNING "Cannot allocate device numer");
        return err;
    }

    cdev_init(&pi_cdev, &pi_fops);
    pi_cdev.owner = THIS_MODULE;
    
    int ret = cdev_add(&pi_cdev, pi_dev_num, MINOR_COUNT);
    if(ret != 0){
        pr_warn("Cannot add cdev");
        goto unregister_cdev;
    }

    pi_class = class_create(CLASS_NAME);
    if(IS_ERR(pi_class)){
        pr_warn("Cannot create class");
        goto destroy_cdev;
    }

    pi_device = device_create(pi_class, NULL, pi_dev_num, NULL, DRIVE_NAME);
    if(IS_ERR(pi_device)){
        pr_warn("Cannot add device file");
        goto destroy_class;
    }

    pr_info("PI GPIO driver loaded. Major: %d, Minor: %d\n", MAJOR(pi_dev_num), MINOR(pi_dev_num));
    return 0;

unregister_cdev:
    unregister_chrdev_region(pi_dev_num, MINOR_COUNT);
    return ret;

destroy_cdev:
    cdev_del(&pi_cdev);
    return IS_ERR(pi_class);      

destroy_class:
    class_destroy(pi_class);
    return IS_ERR(pi_device); 
}

void __exit pi_driver_exit(){
    device_destroy(pi_class, pi_dev_num);
    class_destroy(pi_class);
    cdev_del(&pi_cdev);
    unregister_chrdev_region(pi_dev_num, MINOR_COUNT);
    printk(KERN_INFO "Removed PI Driver");
}

int pi_open(struct inode *, struct file *){
    return 0;
}

int pi_release(struct inode *, struct file *){
    return 0;
}

ssize_t pi_read (struct file *, char __user *, size_t, loff_t *){
    return 1;
}

ssize_t pi_write (struct file *, const char __user *, size_t, loff_t *){
    return 1;
}

module_init(pi_driver_init);
module_exit(pi_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("huyanhh195");
MODULE_DESCRIPTION("Control GPIO_PI_4");