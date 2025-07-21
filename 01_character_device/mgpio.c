#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 0, 0)
    #define BUILD_PI
#endif

#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/device.h> 
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#ifdef BUILD_PI
    #include <linux/io.h>
    #define GPIO_PIN_23 23
    #define BASE_ADDR 0xFE200000
    #define GPFSEL2_OFFSET 0x08
    #define GPSET0_OFFSET 0x1C
    void gpio_mode(void __iomem *gpio_base, int gpio_pin, int mode);
    static void __iomem *gpio_base;
#endif  

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
    int err = 0, ret = 0;

    err = alloc_chrdev_region(&pi_dev_num, MINOR_BASE, MINOR_COUNT, DRIVE_NAME);
    if (err != 0){
        printk(KERN_WARNING "Cannot allocate device numer");
        return err;
    }

    cdev_init(&pi_cdev, &pi_fops);
    pi_cdev.owner = THIS_MODULE;
    
    ret = cdev_add(&pi_cdev, pi_dev_num, MINOR_COUNT);
    if(ret != 0){
        pr_warn("Cannot add cdev");
        goto unregister_cdev;
    }

#ifdef BUILD_PI
    pi_class = class_create(THIS_MODULE, CLASS_NAME);
#else
    pi_class = class_create(CLASS_NAME);
#endif

    if(IS_ERR(pi_class)){
        pr_warn("Cannot create class");
        goto destroy_cdev;
    }

    pi_device = device_create(pi_class, NULL, pi_dev_num, NULL, DRIVE_NAME);
    if(IS_ERR(pi_device)){
        pr_warn("Cannot add device file");
        goto destroy_class;
    }

#ifdef BUILD_PI
    
    int value_register = 0;
    
    gpio_base = ioremap(BASE_ADDR, 0x1000);
    if (!gpio_base) {
        pr_err("ioremap failed\n");
        return -ENOMEM;
    }
    
    value_register = ioread32(gpio_base + GPFSEL2_OFFSET);
    value_register &= ~(7 << 9);
    value_register |= (1 << 9);

    iowrite32(value_register, gpio_base + GPFSEL2_OFFSET);
    iowrite32((1 << 23), gpio_base + GPSET0_OFFSET);

#endif

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
#ifdef BUILD_PI
    if (gpio_base){
        iowrite32((1 << 23), gpio_base + GPCLR0_OFFSET);
        iounmap(gpio_base);
    }
#endif
    printk(KERN_INFO "Removed PI Driver");
}

int pi_open(struct inode *, struct file *){
    return 0;
}

int pi_release(struct inode *, struct file *){
    return 0;
}

ssize_t pi_read(struct file *, char __user *, size_t, loff_t *){

    return 1;
}

ssize_t pi_write(struct file *, const char __user *, size_t, loff_t *){
    
    return 1;
}

#ifdef BUILD_PI
void gpio_mode(void __iomem *gpio_base, int gpio_pin, int mode){
    
}

static int __init mygpio_init(void)
{
    unsigned int reg;

    gpio_base = ioremap(BASE_ADDR, SZ_4K);
    if (!gpio_base) {
        pr_err("Failed to map GPIO\n");
        return -ENOMEM;
    }

    reg = ioread32(gpio_base + GPFSEL2_OFFSET);
    reg &= ~(7 << ((21 % 10) * 3));
    reg |=  (1 << ((21 % 10) * 3));
    iowrite32(reg, gpio_base + GPFSEL2_OFFSET);

    // Bật LED
    iowrite32((1 << 21), gpio_base + GPSET0_OFFSET);

    pr_info("GPIO21 set as output and LED ON\n");
    return 0;
}
#endif 

module_init(pi_driver_init);
module_exit(pi_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("huyanhh195");
MODULE_DESCRIPTION("Control GPIO_PI_4");