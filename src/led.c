
#include <zephyr/kernel.h>
#include <zephyr/drivers/led_strip.h>

#include "led.h"

#define STRIP_NODE		        DT_ALIAS(led_strip)
#define RGB(_r, _g, _b)         { .r = (_r), .g = (_g), .b = (_b) }

static const struct device *const strip = DEVICE_DT_GET(STRIP_NODE);

static const struct led_rgb rgb_colors[] = {
    [COLOR_NONE]    = RGB(0x00, 0x00, 0x00),
    [COLOR_RED]     = RGB(0x3F, 0x00, 0x00),
    [COLOR_GREEN]   = RGB(0x00, 0x3F, 0x00),
    [COLOR_BLUE]    = RGB(0x00, 0x00, 0x3F),
    [COLOR_CYAN]    = RGB(0x00, 0x3F, 0x3F),
    [COLOR_MAGENTA] = RGB(0x3F, 0x00, 0x3F),
};

struct led_t
{
    unsigned int count;
    unsigned int brightness;

    struct led_rgb leds[LED_MAX_COUNT];
};

struct led_t * led_init(int count)
{
    static struct led_t instances[1] = { 0 };
    static unsigned int instance_id;

    struct led_t * self = &instances[instance_id++];
    self->count = count;

	if (!device_is_ready(strip)) {
        return NULL;
	}

    return self;
}

void led_set_color(struct led_t * self, unsigned int id, enum colors_e color)
{
    memcpy(&self->leds[id], &rgb_colors[color], sizeof(struct led_rgb));
}

void led_set_brightness(struct led_t * self, unsigned int brightness)
{
    self->brightness = brightness;
}

void led_reset(struct led_t * self)
{
    memset(&self->leds, 0x00, sizeof(self->leds));
}

void led_update(struct led_t * self)
{
    led_strip_update_rgb(strip, self->leds, self->count);
}
