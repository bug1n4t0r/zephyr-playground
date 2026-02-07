#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

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

  printk("\r\n[UART TX]: Hello from Zephyr");

  while (1) {
    static bool logON = false;
    int         v     = gpio_pin_get_dt(&btn);

    if (v >= 0) {
      gpio_pin_set_dt(&led, v);

      if ((v >= 1) && (!logON)) {
        logON = true;
        printk("\r\n[UART TX]: LED ON! %i", (int32_t)v);
      } else if ((v == 0) && (logON)) {
        logON = false;
        printk("\r\n[UART TX]: LED OFF! %i", (int32_t)v);
      }
    }

    k_sleep(K_MSEC(100));
  }

  return 0;
}
