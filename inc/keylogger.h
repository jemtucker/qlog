#ifndef __KEYLOGGER_H_
#define __KEYLOGGER_H_

/**
 * Register for callbacks from the keyboard driver
 */
int keylogger_register(void);

/**
 * Unregister for callbacks from the keyboard driver
 */
void keylogger_unregister(void);

/**
 * Return the last N keypress events, flushing them from the internal
 * buffer.
 * Returns zero on success otherwise an error code.
 */
//int keylogger_last_events(unsigned int* buf, size_t len);

#endif
