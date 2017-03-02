
#ifndef TEST
    // All kernel-mode-only imports
    #include <linux/init.h>
    #include <linux/module.h>
    #include <linux/kernel.h>

    #define LOG_INFO(...)  (printk(KERN_INFO __VA_ARGS__))
    #define LOG_ERROR(...) (printk(KERN_ALERT __VA_ARGS__))
#else
    #define MODULE_LICENSE(x)
    #define MODULE_AUTHOR(x)
    #define MODULE_DESCRIPTION(x)
    #define MODULE_VERSION(x) 

    #define LOG_INFO(...)  (printf("Info:  " __VA_ARGS__))
    #define LOG_ERROR(...) (printf("Error: " __VA_ARGS__))


    // TODO: Define usermode versions of all funtions for testing
#endif
