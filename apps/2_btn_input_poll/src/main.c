#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

#if !DT_NODE_HAS_STATUS(DT_ALIAS(led0), okay)
  #error "Board does not define alias 'led0'. As a board overlay"
#endif

#if !DT_NODE_HAS_STATUS(DT_ALIAS(sw0), okay)
  #error "Board does not define alias 'sw0'. As a board overlay"
#endif

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec btn = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);

int main(void) {
  if (!device_is_ready(led.port) || !device_is_ready(btn.port)) {
    return 0;
  }

  if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE) != 0) {
    return 0;
  }

  if (gpio_pin_configure_dt(&btn, GPIO_INPUT) != 0) {
    return 0;
  }

  while (1) {
    int v = gpio_pin_get_dt(&btn);

    if (v >= 0) {
      gpio_pin_set_dt(&led, v);
    }

    k_sleep(K_MSEC(10));
  }

  return 0;
}
