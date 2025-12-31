#ifndef LED_H
#define LED_H

#include "colors.h"

#define LED_MAX_COUNT   198

struct led_t;

struct led_t * led_init(int count);
void led_set_color(struct led_t * self, unsigned int id, enum colors_e color);
void led_set_brightness(struct led_t * self, unsigned int brightness);
void led_reset(struct led_t * self);
void led_update(struct led_t * self);

#endif // LED_H