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

static struct adc_channel_cfg ain1_ch1_cfg = {
    .gain             = ADC_GAIN_1,
    .reference        = ADC_REF_INTERNAL,
    .acquisition_time = ADC_ACQ_TIME_DEFAULT,
    .channel_id       = 1,
    .differential     = 0,
};

static struct adc_channel_cfg ain1_ch4_cfg = {
    .gain             = ADC_GAIN_1,
    .reference        = ADC_REF_INTERNAL,
    .acquisition_time = ADC_ACQ_TIME_DEFAULT,
    .channel_id       = 4,
    .differential     = 0,
};

static struct adc_channel_cfg ain1_ch8_cfg = {
    .gain             = ADC_GAIN_1,
    .reference        = ADC_REF_INTERNAL,
    .acquisition_time = ADC_ACQ_TIME_DEFAULT,
    .channel_id       = 8,
    .differential     = 0,
};

static uint16_t adc_sample_buffer;

static struct adc_sequence adc_secqu = {
    .channels    = 0,
    .buffer      = &adc_sample_buffer,
    .buffer_size = sizeof(adc_sample_buffer),
    .resolution  = 12,
};

static int read_one(uint8_t ch, uint16_t *buf) {
  adc_secqu.channels = BIT(ch);
  int ret            = adc_read(adc_dev, &adc_secqu);
  if (ret == 0) {
    *buf = adc_sample_buffer;
  }
  return ret;
}

int main(void) {

  if (!device_is_ready(adc_dev)) {
    printk("ADC device not ready\n");
    return 1;
  }

  if (adc_channel_setup(adc_dev, &ain1_ch1_cfg) ||
      adc_channel_setup(adc_dev, &ain1_ch4_cfg) ||
      adc_channel_setup(adc_dev, &ain1_ch8_cfg)) {
    printk("Failed to setup ADC channels\n");
    return 1;
  }

  while (1) {

    uint16_t v1, v4, v8;

    int ret = read_one(1, &v1);
    if (ret != 0) {
      printk("Failed to read ADC channel 1\n");
      return 1;
    }

    ret = read_one(4, &v4);
    if (ret != 0) {
      printk("Failed to read ADC channel 4\n");
      return 1;
    }

    ret = read_one(8, &v8);
    if (ret != 0) {
      printk("Failed to read ADC channel 8\n");
      return 1;
    }

    double d1 = 3.3 / 4069 * v1;
    double d4 = 3.3 / 4069 * v4;
    double d8 = 3.3 / 4069 * v8;

    printk("ADC values: CH1: %d (%.2f V), CH4: %d (%.2f V), CH8: %d (%.2f V)\n",
           v1, d1, v4, d4, v8, d8);

    k_sleep(K_MSEC(1000));
  }

  return 0;
}
