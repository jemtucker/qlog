// Helper macros for logging.
#define LOG_INFO(...)  (printk(KERN_INFO __VA_ARGS__))
#define LOG_ERROR(...) (printk(KERN_ALERT __VA_ARGS__))
