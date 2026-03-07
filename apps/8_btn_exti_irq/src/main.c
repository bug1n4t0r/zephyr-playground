#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#if !DT_NODE_HAS_STATUS(DT_ALIAS(sw0), okay)
  #error "Board does not define alias 'sw0'. As a board overlay"
#endif

static const struct gpio_dt_spec btn = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
static struct gpio_callback      btn_cb;

static void btn_isr(const struct device *port, struct gpio_callback *cb,
                    gpio_port_pins_t pins) {
  ARG_UNUSED(port);
  ARG_UNUSED(cb);
  ARG_UNUSED(pins);

  // Do Something..
  int v = gpio_pin_get_dt(&btn);

  if (v >= 0) {
    printk("Button state: %d from IRQ_Callback!\n", v);
  }
}

int main(void) {

  if (!device_is_ready(btn.port)) {
    return 0;
  }

  if (gpio_pin_configure_dt(&btn, GPIO_INPUT) != 0) {
    return 0;
  }

  gpio_init_callback(&btn_cb, &btn_isr, BIT(btn.pin));
  gpio_add_callback(btn.port, &btn_cb);

  if (gpio_pin_interrupt_configure_dt(&btn, GPIO_INT_EDGE_BOTH) != 0) {
    return 0;
  }

  printk("\r\n Project Ready! Press Blue Button!\n");

  while (1) {

    k_sleep(K_FOREVER);
  }
}
