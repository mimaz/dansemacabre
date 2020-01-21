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
static struct led_rgb fixed_colors[N_COLORS];
static volatile bool strip_dirty;

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
	strip_dirty = true;
}

static void make_rgb(struct led_rgb *rgb, int r, int g, int b)
{
	rgb->r = r;
	rgb->g = g;
	rgb->b = b;
}

static void strip_update_thread_entry(void *p1, void *p2, void *p3)
{
	while (1) {
		if (strip_dirty) {
			led_strip_update_rgb(strip_driver,
					     strip_colors, LED_COUNT);
			strip_dirty = false;
		}
		k_sleep(K_MSEC(25));
	}
}

K_THREAD_DEFINE(strip_update_thread, 1024,
		strip_update_thread_entry,
		NULL, NULL, NULL,
		1, 0, K_NO_WAIT);

void main(void)
{
	int offset, counter, i;

	strip_driver = device_get_binding(DT_INST_0_WORLDSEMI_WS2812_LABEL);

	make_rgb(&fixed_colors[BLACK], 0, 0, 0);
	make_rgb(&fixed_colors[RED], 255, 0, 0);
	make_rgb(&fixed_colors[GREEN], 0, 255, 0);
	make_rgb(&fixed_colors[BLUE], 0, 0, 255);
	make_rgb(&fixed_colors[YELLOW], 240, 240, 0);
	make_rgb(&fixed_colors[PURPLE], 240, 0, 240);
	make_rgb(&fixed_colors[CYAN], 0, 240, 240);
	make_rgb(&fixed_colors[WHITE], 225, 225, 225);

	counter = 0;

	while (1) {
		/* reset all colors */
		one_color(&fixed_colors[BLACK], -1, -1);

		/* draw moving rainbow */
		for (i = 0; i < N_COLORS; i++) {
			offset = (counter + i) % LED_COUNT;
			one_color(&fixed_colors[i], offset, offset + 1);
		}

		update_strip();

		/* increment the counter and strip then wait 200ms */
		counter++;
		k_sleep(K_MSEC(250));
	}
}
