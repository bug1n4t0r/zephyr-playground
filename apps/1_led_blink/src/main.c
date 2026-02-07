#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#if !DT_NODE_HAS_STATUS(DT_ALIAS(led0), okay)
#error "Board does not define alias 'led0'. Ass a board overlay"
#endif

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

int main(void)
{
    if (!device_is_ready(led.port))
    {
        return 0;
    }

    if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE) != 0)
    {
        return 0;
    }

    while (1)
    {
        gpio_pin_toggle_dt(&led);
        k_sleep(K_MSEC(1000));
    }

    return 0;
}
