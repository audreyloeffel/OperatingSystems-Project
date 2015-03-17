#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/kfifo.h>
#include <asm/semaphore.h>
#include "uart16550.h"
#include "uart16550_hw.h"

MODULE_DESCRIPTION("Uart16550 driver");
MODULE_LICENSE("GPL");

#ifdef __DEBUG
#define dprintk(fmt, ...)     printk(KERN_DEBUG "%s:%d " fmt, \
__FILE__, __LINE__, ##__VA_ARGS__)
#else
#define dprintk(fmt, ...)     do { } while (0)
#endif

static struct class *uart16550_class = NULL;

struct file_operations *fops;

static int major = 42;
static int behaviour = OPTION_BOTH;

module_param(major, int, S_IRUGO);
module_param(behaviour, int, S_IRUGO);

static int Device_Open = 0;

static struct semaphore inmutex;
static struct semaphore outmutex;
DECLARE_MUTEX(inmutex);
DECLARE_MUTEX(outmutex);

static struct kfifo inbuffer;
static struct kfifo outbuffer;
DECLARE_KFIFO(inbuffer, char, FIFO_SIZE);
DECLARE_KFIFO(outbuffer, char, FIFO_SIZE);

static int uart16550_open (struct inode * inode, struct file * file){
    // TODO
    if (Device_Open) return -EBUSY;
    Device_Open++;
    
    MOD_INC_USE_COUNT;
    
    //inode->i_rdev = ;
    
    return SUCCESS;
}

static ssize_t uart16550_read (struct file * file, char __user * buffer, size_t length, loff_t *offset){
    // TODO
    int bytes_read = 0;
    
    down_interruptible(&inmutex);
    
    while (length && *msg_Ptr)  {
        put_user(/* BLABLA buffer IN */, buffer++);
        
        length--;
        bytes_read++;
    }
    
    up_interruptible(&inmutex);
    
    return bytes_read;
}

static int uart16550_release (struct inode * inode, struct file * file){
    // TODO
    Device_Open--;
    
    MOD_DEC_USE_COUNT;
    
    return SUCCESS;
}

static long uart16550_unlocked_ioctl (struct file * file, unsigned int cmd, unsigned long arg){
    // TODO
}

static int uart16550_write(struct file *file, const char *user_buffer,
                           size_t size, loff_t *offset)
{
    int bytes_copied = 0;
    uint32_t device_port;
    /*
     * TODO: Write the code that takes the data provided by the
     *      user from userspace and stores it in the kernel
     *      device outgoing buffer.
     * TODO: Populate bytes_copied with the number of bytes
     *      that fit in the outgoing buffer.
     */
    
    uart16550_hw_force_interrupt_reemit(device_port);
    
    return bytes_copied;
}

irqreturn_t interrupt_handler(int irq_no, void *data)
{
    int device_status;
    uint32_t device_port;
    /*
     * TODO: Write the code that handles a hardware interrupt.
     * TODO: Populate device_port with the port of the correct device.
     */
    
    device_status = uart16550_hw_get_device_status(device_port);
    
    while (uart16550_hw_device_can_send(device_status)) {
        uint8_t byte_value;
        /*
         * TODO: Populate byte_value with the next value
         *      from the kernel device outgoing buffer.
         */
        uart16550_hw_write_to_device(device_port, byte_value);
        device_status = uart16550_hw_get_device_status(device_port);
    }
    
    while (uart16550_hw_device_has_data(device_status)) {
        uint8_t byte_value;
        byte_value = uart16550_hw_read_from_device(device_port);
        /*
         * TODO: Store the read byte_value in the kernel device
         *      incoming buffer.
         */
        device_status = uart16550_hw_get_device_status(device_port);
    }
    
    return IRQ_HANDLED;
}

static int uart16550_init(void)
{
    int have_com1, have_com2;
    
    int ret1, ret2;
    if(behaviour == OPTION_COM1) {
        have_com1 = 1;
        have_com2 = 0;
    }
    else if(behaviour == OPTION_COM2) {
        have_com1 = 0;
        have_com2 = 1;
    }
    else if(behaviour == OPTION_BOTH) {
        have_com1 = 1;
        have_com2 = 1;
    }
    
    // initialize fops...
    
    struct cdev *my_cdev = cdev_alloc();
    cdev_init(&my_cdev, &fops);
    
    /*
     * Setup a sysfs class & device to make /dev/com1 & /dev/com2 appear.
     */
    uart16550_class = class_create(THIS_MODULE, "uart16550");
    
    if (have_com1) {
        ret1 = register_chrdev_region(0, 1, THIS_MODULE->name);
        cdev_add(&my_cdev, 0, 1);
        /* Setup the hardware device for COM1 */
        uart16550_hw_setup_device(COM1_BASEPORT, THIS_MODULE->name);
        /* Create the sysfs info for /dev/com1 */
        device_create(uart16550_class, NULL, MKDEV(major, 0), NULL, "com1");
    }
    if (have_com2) {
        ret2 = register_chrdev_region(1, 1, THIS_MODULE->name);
        cdev_add(&my_cdev, 1, 1);
        /* Setup the hardware device for COM2 */
        uart16550_hw_setup_device(COM2_BASEPORT, THIS_MODULE->name);
        /* Create the sysfs info for /dev/com2 */
        device_create(uart16550_class, NULL, MKDEV(major, 1), NULL, "com2");
    }
    if(have_com1 == 1 && have_com2 == 1){
        if(ret1 != 0) {
            unregister_chrdev_region(1, 1);
        }
        else if(ret2 != 0) {
            unregister_chrdev_region(0, 1);
        }
    }
    return 0;
}

static void uart16550_cleanup(void)
{
    int have_com1, have_com2;
    
    if(behaviour == OPTION_COM1) {
        have_com1 = 1;
        have_com2 = 0;
    }
    else if(behaviour == OPTION_COM2) {
        have_com1 = 0;
        have_com2 = 1;
    }
    else if(behaviour == OPTION_BOTH) {
        have_com1 = 1;
        have_com2 = 1;
    }
    
    if (have_com1) {
        int ret1 = unregister_chrdev_region(0, 1);
        /* Reset the hardware device for COM1 */
        uart16550_hw_cleanup_device(COM1_BASEPORT);
        /* Remove the sysfs info for /dev/com1 */
        device_destroy(uart16550_class, MKDEV(major, 0));
    }
    if (have_com2) {
        int ret2 = unregister_chrdev_region(1, 1);
        /* Reset the hardware device for COM2 */
        uart16550_hw_cleanup_device(COM2_BASEPORT);
        /* Remove the sysfs info for /dev/com2 */
        device_destroy(uart16550_class, MKDEV(major, 1));
    }
    if (ret1 != 0) {
        printk("Error when unregistering device 1: ", ret1);
    }
    if (ret2 != 0) {
        printk("Error when unregistering device 2: ", ret2);
    }
    
    cdev_del(&my_cdev);
    
    /*
     * Cleanup the sysfs device class.
     */
    class_unregister(uart16550_class);
    class_destroy(uart16550_class);
}

module_init(uart16550_init)
module_exit(uart16550_cleanup)
