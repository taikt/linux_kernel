/*
 *  chardev.c : Create a read-only char device that says 
 *  how many times you've read from the dev file
 *  sudo insmod chardev.ko
 *  sudo rmmod chardev
 *  check device file: ls /dev/chardev
 *  read device file: cat /dev/chardev
 *  write to device file: echo "hi" > /dev/chardev 
 */


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/irq.h>
#include <linux/poll.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");

/*
 *  Prototype: This would in a .h file
 *
 */

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "chardev" // appear in /proc/devices
#define BUF_LEN 80 // Max length of the message from the device

/*
 *  Global varialbes are static, so only within the file
 */
static int Major; // assigned to our device driver
static int Device_Open = 0; // prevent multiple access to device
static char msg[BUF_LEN];
static char *msg_ptr;

static struct class *cls;

static struct file_operations chardev_fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

int init_module(void) {
    Major = register_chrdev(0, DEVICE_NAME, &chardev_fops);
    if (Major < 0) {
        pr_alert("register char device failed %d\n", Major);
        return Major;
    }
    pr_info("I was assigned major number %d\n", Major);
    cls = class_create(THIS_MODULE, DEVICE_NAME);
    device_create(cls, NULL, MKDEV(Major, 0), NULL, DEVICE_NAME);
    pr_info("device craeated on /dev/%s\n",DEVICE_NAME);
    return SUCCESS;
}

void cleanup_module(void) {
    device_destroy(cls, MKDEV(Major, 0));
    class_destroy(cls);
    unregister_chrdev(Major, DEVICE_NAME);
}

/*
 * Methods
 */
/*
 * Called when a process open the device file
 *  i.e cat /dev/chardev
 */
static int device_open(struct inode *inode, struct file *file) {
    static int counter = 0;
    if (Device_Open) return -EBUSY;

    Device_Open++;
    sprintf(msg, "counter=%d\n",counter++);
    msg_ptr = msg;
    try_module_get(THIS_MODULE);

    return SUCCESS;    
}

/*
 * Called when a process closes the device file
 *
 */
static int device_release(struct inode *inode, struct file *file) {
    Device_Open--;
    module_put(THIS_MODULE);

    return SUCCESS;
}

/*
 * When a process read the dev file
 * filp: check include/linux/fs.h
 * buffer: buffer to fill the data
 * length: length of the buffer
 *
 *
 */
static ssize_t device_read(struct file * filp, char * buffer, size_t length, loff_t *offset) {
    /* have to init variable 
    */
    int bytes_read = 0; // number of bytes written to buffer
    
    if (*msg_ptr == 0) return 0; //end of message
    // put data into the buffer
    while (length && *msg_ptr) {
        /* buffer is in user segment, not kernel segment
         * use put_user to copy data from kernel to user segment
         */
        put_user(*(msg_ptr++), buffer++);
        length--;
        bytes_read++;
    }
    return bytes_read;
}

/* 
 * called when a process write to dev file: echo "hi" > /dev/hello
 */
static ssize_t device_write(struct file *filp, const char* buffer,  size_t length, loff_t* offset) 
{
    pr_alert("write ops is not supported\n");
    return -EINVAL;
}





