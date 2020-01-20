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

static struct k_timer loop_timer;
static struct k_thread loop_thread;

static void loop_handler(struct k_timer *tim)
{
	k_thread_resume(&loop_thread);
}

static void loop_entry(void *p1, void *p2, void *p3)
{
	int offset, counter, i;

	counter = 0;

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
		k_thread_suspend(&loop_thread);
	}
}

K_THREAD_STACK_DEFINE(loop_stack, 500);

void main(void)
{
	strip_driver = device_get_binding(DT_INST_0_WORLDSEMI_WS2812_LABEL);

	make_rgb(&colors[BLACK], 0, 0, 0);
	make_rgb(&colors[RED], 255, 0, 0);
	make_rgb(&colors[GREEN], 0, 255, 0);
	make_rgb(&colors[BLUE], 0, 0, 255);
	make_rgb(&colors[YELLOW], 240, 240, 0);
	make_rgb(&colors[PURPLE], 240, 0, 240);
	make_rgb(&colors[CYAN], 240, 0, 240);
	make_rgb(&colors[WHITE], 225, 225, 225);

	k_thread_create(&loop_thread, 
			loop_stack, K_THREAD_STACK_SIZEOF(loop_stack),
			loop_entry, NULL, NULL, NULL,
			1, 0, K_NO_WAIT);

	k_timer_init(&loop_timer, loop_handler, NULL);
	k_timer_start(&loop_timer, 100, 500);
}
