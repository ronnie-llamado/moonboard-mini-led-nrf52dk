#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <stdbool.h>

void bluetooth_init(void);
char * bluetooth_get_message(void);
bool bluetooth_message_received(void);

#endif // BLUETOOTH_H