/*
 * 2020 Mieszko Mazurek <mimaz@gmx.com> 
 */
#include <drivers/gpio.h>
#include <drivers/led_strip.h>
#include <string.h>
#include <math.h>

#define LED_COUNT 8

enum
{
	BLACK,
	RED,
	GREEN,
	BLUE,
	YELLOW,
	PURPLE,
	CYAN,
	WHITE,
	N_COLORS,
};

static struct device *strip_driver;
static struct led_rgb strip_colors[LED_COUNT];
static struct led_rgb colors[N_COLORS];

static void one_color(struct led_rgb *rgb, int begin, int end)
{
	int i;
	if (end < 0)
		end = LED_COUNT;
	begin = MAX(begin, 0);
	end = MIN(end, LED_COUNT);
	for (i = begin; i < end; i++)
		memcpy(&strip_colors[i], rgb, sizeof(struct led_rgb));
}

static void update_strip()
{
	led_strip_update_rgb(strip_driver, strip_colors, LED_COUNT);
}

static void make_rgb(struct led_rgb *rgb, int r, int g, int b)
{
	rgb->r = r;
	rgb->g = g;
	rgb->b = b;
}

void main(void)
{
	int counter, i, offset;
	time_t nexttime;

	strip_driver = device_get_binding(DT_INST_0_WORLDSEMI_WS2812_LABEL);
	counter = 0;

	make_rgb(&colors[BLACK], 0, 0, 0);
	make_rgb(&colors[RED], 255, 0, 0);
	make_rgb(&colors[GREEN], 0, 255, 0);
	make_rgb(&colors[BLUE], 0, 0, 255);
	make_rgb(&colors[YELLOW], 240, 240, 0);
	make_rgb(&colors[PURPLE], 240, 0, 240);
	make_rgb(&colors[CYAN], 240, 0, 240);
	make_rgb(&colors[WHITE], 225, 225, 225);

	while (1) {
		/* reset all colors */
		one_color(&colors[BLACK], -1, -1);

		/* draw moving rainbow */
		for (i = 0; i < N_COLORS; i++) {
			offset = (counter + i) % LED_COUNT;
			one_color(&colors[i], offset, offset + 1);
		}

		/* increment the counter and strip then wait 200ms */
		counter++;
		update_strip();
		k_sleep(K_MSEC(200));
	}
}
