#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define ADC_NODE DT_NODELABEL(adc1)
#if !DT_NODE_HAS_STATUS(ADC_NODE, okay)
  #error "Unsupported board: adc1 devicetree node is not okay"
#endif

static const struct device *const adc_dev = DEVICE_DT_GET(ADC_NODE);

static struct adc_channel_cfg ain1_ch_cfg = {
    .gain             = ADC_GAIN_1,
    .reference        = ADC_REF_INTERNAL,
    .acquisition_time = ADC_ACQ_TIME_DEFAULT,
    .channel_id       = 1,
    .differential     = 0,
};

static uint16_t adc_sample_buffer;

static struct adc_sequence adc_secqu = {
    .channels     = BIT(1),
    .buffer       = &adc_sample_buffer,
    .buffer_size  = sizeof(adc_sample_buffer),
    .resolution   = 12,
    .oversampling = 0,
    .calibrate    = false,
};

int main(void) {

  if (!device_is_ready(adc_dev)) {
    printk("ADC device not ready\n");
    return 1;
  }

  if (adc_channel_setup(adc_dev, &ain1_ch_cfg) != 0) {
    printk("Failed to setup ADC channel\n");
    return 1;
  }

  while (1) {
    int ret = adc_read(adc_dev, &adc_secqu);
    if (ret != 0) {
      printk("Failed to read ADC\n");
      return 1;
    }
    double v = 3.3 / 4069 * adc_sample_buffer;

    printk("ADC value: Raw: %d  (%.2f V)\n", adc_sample_buffer, v);

    k_sleep(K_MSEC(1000));
  }

  return 0;
}