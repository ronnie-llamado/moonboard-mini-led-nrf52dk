
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/drivers/led_strip.h>

#include "moonboard.h"

#define STRIP_NODE		        DT_ALIAS(led_strip)
#define MOONBOARD_LED_COUNT     150
#define RGB(_r, _g, _b)         { .r = (_r), .g = (_g), .b = (_b) }

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

static const struct led_rgb moonboard_hold_colors[] = {
    [MOONBOARD_HOLD_NONE]       = RGB(0x00, 0x00, 0x00),    // none
    [MOONBOARD_HOLD_START]      = RGB(0x00, 0xFF, 0x00),    // green
    [MOONBOARD_HOLD_REGULAR]    = RGB(0x00, 0x00, 0xFF),    // blue
    [MOONBOARD_HOLD_END]        = RGB(0xFF, 0x00, 0x00),    // red
    [MOONBOARD_HOLD_RIGHT]      = RGB(0x00, 0x00, 0xFF),    // blue
    [MOONBOARD_HOLD_LEFT]       = RGB(0xFF, 0x00, 0xFF),    // magenta
    [MOONBOARD_HOLD_FOOT]       = RGB(0x00, 0xFF, 0xFF),    // cyan
};

static bool message_received = false;
static char message[64];
static enum moonboard_hold_e moonboard[MOONBOARD_LED_COUNT];
static struct led_rgb moonboard_leds[MOONBOARD_LED_COUNT];

static const struct device *const strip = DEVICE_DT_GET(STRIP_NODE);

static void moonboard_parse_message(char * message);
static void moonboard_update_leds(void);
static enum moonboard_hold_e moonboard_hold_from_character(const char * lookup, char c);

enum moonboard_status_e moonboard_init(void)
{
    // TODO: Initialize BLUETOOTH

	if (!device_is_ready(strip)) {
        return MOONBOARD_STATUS_FAILURE_LED;
	}

    // Mock received message
    strcpy(message,"~M*l#R4,E35,R44,R56,R60,S77,R80,S100#\r\n");
    message_received = true;

    return MOONBOARD_STATUS_SUCCESS;
}

void moonboard_process(void)
{
    if (message_received)
    {
        moonboard_parse_message(message);
        moonboard_update_leds();
        message_received = false;
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

static void moonboard_update_leds(void)
{
    enum moonboard_hold_e hold = MOONBOARD_HOLD_NONE;

    memset(&moonboard_leds, 0x00, sizeof(moonboard_leds));

    for(int i=0; i<sizeof(moonboard); i++)
    {
        hold = moonboard[i];

        if(hold != MOONBOARD_HOLD_NONE)
        {
			memcpy(&moonboard_leds[i], &moonboard_hold_colors[hold], sizeof(struct led_rgb));
        }
    }

    led_strip_update_rgb(strip, moonboard_leds, MOONBOARD_LED_COUNT);
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