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

#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <unistd.h>

#define LED_GPIO GPIO_NUM_4
#define UART UART_NUM_0
#define BUF_SIZE (2 * SOC_UART_FIFO_LEN) // SOC_UART_FIFO_LEN
#define MSGLEN BUFLEN

void tempstart(void) {
  // Define the GPIO pin for the LED

  // Define a tag for logging
  static const char *TAG = "LED_CONTROL";

  ESP_LOGI(TAG, "Configuring GPIO...");

  // Reset the pin to a known state before configuring
  gpio_reset_pin(LED_GPIO);

  /*
   * Configure the GPIO pin:
   * - Set the direction to output.
   * - The configuration is encapsulated in the gpio_config_t structure.
   */
  gpio_config_t io_conf = {
      // bit mask of the pins to set, e.g. (1ULL << LED_GPIO)
      .pin_bit_mask = (1ULL << LED_GPIO),
      // set as output mode
      .mode = GPIO_MODE_OUTPUT,
      // disable pull-up mode
      .pull_up_en = GPIO_PULLUP_DISABLE,
      // disable pull-down mode
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      // disable interrupt
      .intr_type = GPIO_INTR_DISABLE};

  // Apply the configuration to the GPIO pin
  ESP_ERROR_CHECK(gpio_config(&io_conf));

  ESP_LOGI(TAG, "Configuration complete.");
}

void app_main() {
  tempstart();
  static const char *TAG = "UART_Example";

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

  ESP_LOGI(TAG, "UART initialized");

  uint8_t buffer[BUF_SIZE] = {0};

  while (1) {
    gpio_set_level(LED_GPIO, 0);
    if (MSGLEN == uart_read_bytes(UART, buffer, MSGLEN, portMAX_DELAY)) {
      ESP_LOGI(TAG, "Received data: %.*s", MSGLEN, buffer);
      gpio_set_level(LED_GPIO, 1);
      usleep(40000); // Sleep 
    } else {
      ESP_LOGE(TAG, "Failed to read");
      gpio_set_level(LED_GPIO, 1);
      usleep(40000); // Sleep
      gpio_set_level(LED_GPIO, 0);
    }
  }
}