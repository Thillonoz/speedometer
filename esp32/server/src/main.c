#include "driver/uart.h"
#include "esp_bt.h"
#include "esp_log.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "nimble/ble.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "nvs_flash.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "setting.h"
#include <stdbool.h>

#define TEMP_TESTING // GPIO testing, comment out to disable
#ifdef TEMP_TESTING

#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <unistd.h>

#define LED_GPIO GPIO_NUM_4

void tempstart(void) {
  static const char *TAG = "LED_CONTROL";

  ESP_LOGI(TAG, "Configuring GPIO...");

  gpio_reset_pin(LED_GPIO);
  gpio_config_t io_conf = {.pin_bit_mask = (1ULL << LED_GPIO),
                           .mode = GPIO_MODE_OUTPUT,
                           .pull_up_en = GPIO_PULLUP_DISABLE,
                           .pull_down_en = GPIO_PULLDOWN_DISABLE,
                           .intr_type = GPIO_INTR_DISABLE};

  ESP_ERROR_CHECK(gpio_config(&io_conf));
}

void tempCheck(uint8_t *_buffer, uint8_t _bool) {
  if (_bool == true) {
    if (_buffer[0] == 0b00000001) { // speed
      gpio_set_level(LED_GPIO, 1);
    } else if (_buffer[1] == 0b00000001) { // temperature
      gpio_set_level(LED_GPIO, 1);
    } else if (_buffer[1] == 0b10000000) { // battery
      gpio_set_level(LED_GPIO, 1);
    } else if (_buffer[2] == 0b01000000) { // left blinker
      gpio_set_level(LED_GPIO, 1);
    } else if (_buffer[2] == 0b10000000) { // right blinker
      gpio_set_level(LED_GPIO, 1);
    } else if (_buffer[2] == 0b11000000) { // warning blinker
      gpio_set_level(LED_GPIO, 1);
    } else {
      gpio_set_level(LED_GPIO, 0);
    }
  } else {
    gpio_set_level(LED_GPIO, 1);
  }
}
#endif

#define UART UART_NUM_0
#define BUF_SIZE (2 * SOC_UART_FIFO_LEN) // SOC_UART_FIFO_LEN
#define MSGLEN BUFLEN

// Setting::INTERVAL, change when setting is updated.
static const int INTERVAL = 40;

void app_main() {
  tempstart();

  uart_config_t config = {
      .baud_rate = BAUDRATE,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .source_clk = UART_SCLK_DEFAULT,
  };

  // Install driver and configure UART
  ESP_ERROR_CHECK(uart_driver_install(UART, BUF_SIZE, 0, 0, NULL, 0));
  ESP_ERROR_CHECK(uart_param_config(UART, &config));

  uint8_t buffer[BUF_SIZE] = {0};

  while (1) {
    if (MSGLEN == uart_read_bytes(UART, buffer, MSGLEN, portMAX_DELAY)) {
#ifdef TEMP_TESTING
      tempCheck(buffer, true);
#endif
    } else {
#ifdef TEMP_TESTING
      tempCheck(buffer, false);
#endif
    }
    vTaskDelay(pdMS_TO_TICKS(INTERVAL));
  }
}