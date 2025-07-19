#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/device.h> 
#include <linux/module.h>
#include <linux/cdev.h>

#define BUFFER_SIZE 256
#define NAME_DRIVER "pi_gpio"
#define BASE_MINOR 0
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
static struct file_operations pi_fops = {
    .owner = THIS_MODULE,
    .open = pi_open,
    .release = pi_release,
    .read = pi_read, 
    .write = pi_write,
};

int __init pi_driver_init(){
    int err = alloc_chrdev_region(&pi_dev_num, BASE_MINOR, MINOR_COUNT, NAME_DRIVER);
    if (err != 0){
        printk(KERN_WARNING "Cannot allocate device numer");
        return err;
    }

    cdev_init(&pi_cdev, &pi_fops);
    
    int ret = cdev_add(&pi_cdev, pi_dev_num, MINOR_COUNT);
    if(ret != 0){
        printk(KERN_WARNING "Cannot add cdev");
        goto unregister_dev;
    }
    
    printk(KERN_INFO "Pi gpio driver loaded. Major: %d, Minor: %d\n", MAJOR(pi_dev_num), MINOR(pi_dev_num));
    return 0;
    
    unregister_dev:
        unregister_chrdev_region(pi_dev_num, MINOR_COUNT);
        return ret;
}

void __exit pi_driver_exit(){
    cdev_del(&pi_cdev);
    printk(KERN_INFO "removed pi driver");
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