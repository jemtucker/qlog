#ifndef __DEFINITIONS_H_
#define __DEFINITIONS_H_

// Helper macros for logging.
#define LOG_DEBUG(...)  (printk(KERN_DEBUG __VA_ARGS__))
#define LOG_INFO(...)  (printk(KERN_INFO __VA_ARGS__))
#define LOG_ERROR(...) (printk(KERN_ALERT __VA_ARGS__))

#endif
