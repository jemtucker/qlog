#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#include "definitions.h"

#define CLASS_NAME "hello"
#define DEVICE_NAME "hellochar"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jem Tucker");
MODULE_DESCRIPTION("A simple Linux driver for fun.");
MODULE_VERSION("0.1");

static int hello_open(struct inode*, struct file*);
static ssize_t hello_read(struct file*, char*, size_t, loff_t*);
static int hello_release(struct inode*, struct file*);

static struct file_operations fops = {
    .open = hello_open,
    .read = hello_read,
    .release = hello_release
};

// Device statics
static int s_major = -1;
static struct device* s_device = NULL;
static struct class* s_class = NULL;
static volatile size_t s_numopen = 0;

/**
 * Kernel module Initialisation function
 * @return Returns 0 if successful
 */ 
static int __init hello_init(void) {
    int retval = 0;

    LOG_INFO("Initialising device - %s\n", DEVICE_NAME);

    // Dynamically allocate a device object major number
    s_major = register_chrdev(
        0,           // Major number - zero for dynamic allocation
        DEVICE_NAME, // The device name to display in /dev/
        &fops        // File operation
        );

    // If failed return the error code. 
    if (s_major < 0) {
        LOG_ERROR("Failed to register device. Errno: %d\n", -s_major);
        retval = s_major;
        goto cleanup;
    }

    // Create the device class
    s_class = class_create(
        THIS_MODULE, // Parent module of the class
        CLASS_NAME   // Class name
        );
    
    // If create_class failed return the error 
    if (IS_ERR(s_class)) {
        retval = PTR_ERR(s_class);
        LOG_ERROR("Failed to create_class. Errno: %d\n", retval);
        s_class = NULL;
        goto cleanup;
    }
    
    // Register the device
    s_device = device_create(
        s_class,           // The device class
        NULL,              // The parent device (we have no parents)
        MKDEV(s_major, 0), // 
        NULL,              // Device data (none to pass)
        DEVICE_NAME        // The device name
        );
    
    // If failed to create the device, return the error
    if (IS_ERR(s_device)) {
        retval = PTR_ERR(s_device);
        LOG_ERROR("Failed call to device_create. Errno: %d\n", retval);
        s_device = NULL;
        goto cleanup;
    }

    // Success.
    LOG_INFO("Device %s registered successfully.", DEVICE_NAME);
    return retval;

cleanup:
    // Destroy the class
    if (s_class) {
        class_destroy(s_class); 
    }
    
    // Unregister the device
    if (s_major >= 0) {
        unregister_chrdev(s_major, DEVICE_NAME);
    }

    return retval;
}

/**
 * Kernel module exit function
 * Free all resources and exit. This function assumes the module has
 * been initialised completely.  
 */
static void __exit hello_exit(void) {
    LOG_INFO("Unloading device %s\n", DEVICE_NAME);

    // Cleanup the device
    device_destroy(s_class, MKDEV(s_major, 0));

    // Unregister the device class
    class_unregister(s_class);

    // Cleanup the device class
    class_destroy(s_class);

    // Unregister the device
    unregister_chrdev(s_major, DEVICE_NAME);
}
 
module_init(hello_init);
module_exit(hello_exit);

/**
 * Open device file operation
 * Returns 0. . 
 */
static int hello_open(struct inode* inode, struct file* file) {
    LOG_INFO("hello_open");
    return 0;
}

/**
 * Device read file operation
 * Returns a message to the caller in 'buf' and the number of bytes copied. On error
 * a negative error code will be returned. 
 */
static ssize_t hello_read(struct file* file, char* buf, size_t len, loff_t* off) {
    const char message[] = "Hello from the kernel!";
    const size_t messageLen = sizeof(message);
    unsigned long errorCount = 0;

    LOG_INFO("hello_read");

    if (len < messageLen) {
        return 0;
    }
    
    // Copy the message to user mode
    errorCount = copy_to_user(
        buf,       // 'To' address in user mode
        message,   // 'From' address in kernel mode
        messageLen // Number of bytes to copy
    );

    // If any bytes could not be copied return an error, otherwise
    // return the length of the message.
    if (errorCount) {
        return -EFAULT;
    } else {
        return messageLen;
    }
}

/**
 * Device release file operation.
 * Returns 0.
 */
static int hello_release(struct inode* inode, struct file* file) {
    LOG_INFO("hello_release");
    return 0;
}

