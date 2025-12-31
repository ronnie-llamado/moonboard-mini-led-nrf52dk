
#include <string.h>

#include "bluetooth.h"

static bool message_received = false;
static char message[64];

void bluetooth_init(void)
{
    message_received = true;
    strcpy(message,"~M*l#R4,E35,R44,R56,R60,S77,R80,S100#\r\n");
}

void bluetooth_process(void)
{

}

char * bluetooth_get_message(void)
{
    return &message[0];
}

bool bluetooth_message_received(void)
{
    bool value = message_received;

    if (value)
    {
        message_received = false;
    }

    return value;
}

