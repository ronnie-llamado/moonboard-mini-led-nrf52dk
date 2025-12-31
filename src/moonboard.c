
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bluetooth.h"
#include "colors.h"
#include "led.h"
#include "moonboard.h"

#define MOONBOARD_LED_COUNT     150

enum moonboard_hold_e
{
    MOONBOARD_HOLD_NONE = 0,
    MOONBOARD_HOLD_START,
    MOONBOARD_HOLD_REGULAR,
    MOONBOARD_HOLD_END,
    MOONBOARD_HOLD_RIGHT,
    MOONBOARD_HOLD_LEFT,
    MOONBOARD_HOLD_FOOT,

    // End
    MOONBOARD_HOLD_MAX,
};

static const char moonboard_hold_letters0[] = {
    [MOONBOARD_HOLD_NONE]       = '\0',
    [MOONBOARD_HOLD_START]      = 'S',
    [MOONBOARD_HOLD_REGULAR]    = 'R',
    [MOONBOARD_HOLD_END]        = 'E',
    [MOONBOARD_HOLD_MAX]        = '\0',
};

static const char moonboard_hold_letters1[] = {
    [MOONBOARD_HOLD_NONE]       = '\0',
    [MOONBOARD_HOLD_START]      = 'S',
    [MOONBOARD_HOLD_END]        = 'E',
    [MOONBOARD_HOLD_RIGHT]      = 'R',
    [MOONBOARD_HOLD_LEFT]       = 'L',
    [MOONBOARD_HOLD_FOOT]       = 'F',
    [MOONBOARD_HOLD_MAX]        = '\0',
};

static const enum colors_e moonboard_hold_colors[] = {
    [MOONBOARD_HOLD_NONE]       = COLOR_NONE,
    [MOONBOARD_HOLD_START]      = COLOR_GREEN,
    [MOONBOARD_HOLD_REGULAR]    = COLOR_BLUE,
    [MOONBOARD_HOLD_END]        = COLOR_RED,
    [MOONBOARD_HOLD_RIGHT]      = COLOR_BLUE,
    [MOONBOARD_HOLD_LEFT]       = COLOR_CYAN,
    [MOONBOARD_HOLD_FOOT]       = COLOR_MAGENTA,
};

static enum moonboard_hold_e moonboard[MOONBOARD_LED_COUNT];

static struct led_t * moonboard_led;

static void moonboard_parse_message(char * message);
static void moonboard_led_update(void);
static enum moonboard_hold_e moonboard_hold_from_character(const char * lookup, char c);

enum moonboard_status_e moonboard_init(void)
{
    bluetooth_init();

    moonboard_led = led_init(MOONBOARD_LED_COUNT);

    return MOONBOARD_STATUS_SUCCESS;
}

void moonboard_process(void)
{
    char * message;
    bool message_received = bluetooth_message_received();

    if (message_received)
    {
        message = bluetooth_get_message();
        moonboard_parse_message(message);
        moonboard_led_update();
    }
}

static void moonboard_parse_message(char * message)
{
    char * token;
    const char * start_of_frame = "~";
    const char * section = "*l#";
    const char * delimiter = ",";
    
    char * frame = strtok(message, start_of_frame);
    
    // Split input into settings and payload
    char * settings = strtok(frame, section);
    char * payload = strtok(NULL, section);
    
    // Parse settings
    token = strtok(settings, delimiter);
    while (token != NULL)
    {
        token = strtok(NULL, delimiter);
    }

    // Parse payload
    token = strtok(payload, delimiter);
    while (token != NULL)
    {
        enum moonboard_hold_e hold = moonboard_hold_from_character(&moonboard_hold_letters0[0], token[0]);

        int number = atoi(&token[1]);
        number -= 1;

        moonboard[number] = hold;

        token = strtok(NULL, delimiter);
    }
}

static void moonboard_led_update(void)
{
    enum moonboard_hold_e hold = MOONBOARD_HOLD_NONE;

    led_reset(moonboard_led);

    for(int i=0; i<sizeof(moonboard); i++)
    {
        hold = moonboard[i];

        if(hold != MOONBOARD_HOLD_NONE)
        {
            led_set_color(moonboard_led, i, moonboard_hold_colors[hold]);
        }
    }

    led_update(moonboard_led);
}

static enum moonboard_hold_e moonboard_hold_from_character(const char * lookup, char c)
{
    enum moonboard_hold_e hold = MOONBOARD_HOLD_NONE;

    int i = 0;
    while (lookup[i] != MOONBOARD_HOLD_MAX)
    {
        if (lookup[i] == c)
        {
            hold = (enum moonboard_hold_e)i;
            break;
        }
        
        i++;
    }

    return hold;
}
