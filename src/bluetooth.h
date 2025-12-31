#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <stdbool.h>

struct bluetooth_t;

struct bluetooth_t * bluetooth_init(void);
char * bluetooth_get_message(struct bluetooth_t * self);
bool bluetooth_message_received(struct bluetooth_t * self);

#endif // BLUETOOTH_H