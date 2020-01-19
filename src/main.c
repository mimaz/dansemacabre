#include <drivers/gpio.h>
#include <drivers/led_strip.h>
#include <string.h>

#define LED_COUNT 8

static void set_rgb(struct led_rgb *rgb, int r, int g, int b)
{
	rgb->r = r;
	rgb->g = g;
	rgb->b = b;
}

void main(void)
{
	struct device *gpio, *strip;
	struct led_rgb colors[LED_COUNT];
	int cnt;

	gpio = device_get_binding(DT_ALIAS_LED0_GPIOS_CONTROLLER);
	strip = device_get_binding(DT_INST_0_WORLDSEMI_WS2812_LABEL);
	cnt = 0;

	gpio_pin_configure(gpio, DT_ALIAS_LED0_GPIOS_PIN, GPIO_DIR_OUT);

	while (1) {
		memset(colors, 0, sizeof(colors));
		set_rgb(&colors[(cnt + 0) % LED_COUNT], 255, 0, 0);
		set_rgb(&colors[(cnt + 1) % LED_COUNT], 0, 255, 0);
		set_rgb(&colors[(cnt + 2) % LED_COUNT], 0, 0, 255);
		led_strip_update_rgb(strip, colors, LED_COUNT);

		k_sleep(K_MSEC(250));
		cnt++;
	}
}
