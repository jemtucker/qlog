#include "definitions.h"
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jem Tucker");
MODULE_DESCRIPTION("A simple Linux driver for fun.");
MODULE_VERSION("0.1");

/**
 * Kernel module Initialisation function
 * @return Returns 0 if successful
 */ 
static int __init hello_init(void){
    LOG_INFO("Hello World!\n");
    return 0;
}

/**
 * Kernel module exit function
 * Free all resources and exit. 
 */
static void __exit hello_exit(void){
    LOG_INFO("Goodby %s!\n", "World");
}
 
module_init(hello_init);
module_exit(hello_exit);
