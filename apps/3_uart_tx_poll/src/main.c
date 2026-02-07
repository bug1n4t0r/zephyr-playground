#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>

#define UART_NODE DT_CHOSEN(zephyr_console)

#if !DT_NODE_HAS_STATUS(UART_NODE, okay)
  #error "UART console not aviable. As a board overlay"
#endif

#if !DT_NODE_HAS_STATUS(DT_ALIAS(led0), okay)
  #error "Board does not define alias 'led0'. As a board overlay"
#endif

#if !DT_NODE_HAS_STATUS(DT_ALIAS(sw0), okay)
  #error "Board does not define alias 'sw0'. As a board overlay"
#endif

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec btn = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);

static const struct device *const uart_dev = DEVICE_DT_GET(UART_NODE);

static void uart_tx_str(const char *s) {
  while (*s) {
    uart_poll_out(uart_dev, (uint8_t)*s++);
  }
}

int main(void) {
  if (!device_is_ready(uart_dev)) {
    return 0;
  }

  if (!device_is_ready(led.port) || !device_is_ready(btn.port)) {
    return 0;
  }

  if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE) != 0) {
    return 0;
  }

  if (gpio_pin_configure_dt(&btn, GPIO_INPUT) != 0) {
    return 0;
  }

  uart_tx_str("\r\n[UART TX]: Hello from Zephyr");

  while (1) {
    int v = gpio_pin_get_dt(&btn);

    if (v >= 0) {
      gpio_pin_set_dt(&led, v);

      if (v >= 1) {
        char buf[32];
        snprintf(buf, sizeof(buf), "\r\n[UART TX]: LED ON! %i", (int32_t)v);
        uart_tx_str(buf);
      }
    }

    k_sleep(K_MSEC(10));
  }

  return 0;
}
