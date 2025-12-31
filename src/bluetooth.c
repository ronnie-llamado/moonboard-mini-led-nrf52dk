
#include <string.h>

#include "bluetooth.h"

struct bluetooth_t
{
    bool message_received;
    char message[64];
};

struct bluetooth_t * bluetooth_init(void)
{
    static struct bluetooth_t instances[1];
    static unsigned int instance_id = 0;

    struct bluetooth_t * self = &instances[instance_id++];

    self->message_received = true;
    strcpy(self->message,"~M*l#R4,E35,R44,R56,R60,S77,R80,S100#\r\n");

    return self;
}

char * bluetooth_get_message(struct bluetooth_t * self)
{
    return &self->message[0];
}

bool bluetooth_message_received(struct bluetooth_t * self)
{
    bool value = self->message_received;

    if (value)
    {
        self->message_received = false;
    }

    return value;
}

