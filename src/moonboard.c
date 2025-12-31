
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/drivers/led_strip.h>

#include "moonboard.h"

#define STRIP_NODE		        DT_ALIAS(led_strip)
#define MOONBOARD_LED_COUNT     150
#define RGB(_r, _g, _b)         {.r=_r, .g=_g, .b=_b}

enum moonboard_hold_e
{
    MOONBOARD_HOLD_NONE = 0,
    MOONBOARD_HOLD_START,
    MOONBOARD_HOLD_REGULAR,
    MOONBOARD_HOLD_END,
    MOONBOARD_HOLD_RIGHT,
    MOONBOARD_HOLD_LEFT,
    MOONBOARD_HOLD_FOOT,
};

static const char moonboard_hold_letters0[] = {
    [MOONBOARD_HOLD_NONE]       = '\0',
    [MOONBOARD_HOLD_START]      = 'S',
    [MOONBOARD_HOLD_REGULAR]    = 'R',
    [MOONBOARD_HOLD_END]        = 'E',
};

static const char moonboard_hold_letters1[] = {
    [MOONBOARD_HOLD_NONE]       = '\0',
    [MOONBOARD_HOLD_START]      = 'S',
    [MOONBOARD_HOLD_END]        = 'E',
    [MOONBOARD_HOLD_RIGHT]      = 'R',
    [MOONBOARD_HOLD_LEFT]       = 'L',
    [MOONBOARD_HOLD_FOOT]       = 'F',
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

static bool update = false;
static enum moonboard_hold_e moonboard[MOONBOARD_LED_COUNT];
static struct led_rgb moonboard_leds[MOONBOARD_LED_COUNT];

static const struct device *const strip = DEVICE_DT_GET(STRIP_NODE);

static void moonboard_update(void);

enum moonboard_status_e moonboard_init(void)
{
    // TODO: Initialize BLUETOOTH
    memset(&moonboard, MOONBOARD_HOLD_NONE, sizeof(moonboard));

	if (!device_is_ready(strip)) {
        return MOONBOARD_STATUS_LED_FAILURE;
	}

    moonboard[0] = MOONBOARD_HOLD_START;
    moonboard[1] = MOONBOARD_HOLD_REGULAR;
    moonboard[2] = MOONBOARD_HOLD_END;
    moonboard[3] = MOONBOARD_HOLD_RIGHT;
    moonboard[4] = MOONBOARD_HOLD_LEFT;
    moonboard[5] = MOONBOARD_HOLD_FOOT;

    update = true;

    return MOONBOARD_STATUS_SUCCESS;
}

void moonboard_process(void)
{
    if (update)
    {
        moonboard_update();
    }
}

static void moonboard_update(void)
{
    enum moonboard_hold_e moonboard_hold = MOONBOARD_HOLD_NONE;

    memset(&moonboard_leds, 0x00, sizeof(moonboard_leds));

    for(int i=0; i<sizeof(moonboard); i++)
    {
        moonboard_hold = moonboard[i];

        if(moonboard_hold != MOONBOARD_HOLD_NONE)
        {
			memcpy(&moonboard_leds[i], &moonboard_hold_colors[moonboard_hold], sizeof(struct led_rgb));
        }
    }

    led_strip_update_rgb(strip, moonboard_leds, MOONBOARD_LED_COUNT);
}