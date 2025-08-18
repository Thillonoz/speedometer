#include <stdio.h>
#include <stdlib.h>
#include "setting.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/uart.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "nimble/nimble_port.h"
#include "services/gap/ble_svc_gap.h"
#include "nimble/nimble_port_freertos.h"

#include "driver/gpio.h"

void app_main(void)
{

    // TEMP LED to test
    ESP_ERROR_CHECK(gpio_reset_pin(GPIO_NUM_4));

    ESP_ERROR_CHECK(gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT)); // Configure pin 4 as a digital output pin

    uint32_t state = 0;

    // UART config
    uart_config_t uart_config = {
        .baud_rate = BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_driver_install(UART, 256, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART, &uart_config));
    // (Optional) Set pins if needed: uart_set_pin(UART, TX, RX, RTS, CTS);

    // Example test values
    uint8_t speed = 120;         // 0–240
    int8_t temperature = 25;     // -60–60
    uint8_t battery = 80;        // 0–100
    uint8_t left = 1;            // on
    uint8_t right = 0;           // off

    uint8_t temp_enc = (uint8_t)(temperature + 60); // offset for signed

    uint32_t packed = 0;
    packed |= ((uint32_t)speed & 0xFF) << 0;           // bits 0-7
    packed |= ((uint32_t)temp_enc & 0x7F) << 8;        // bits 8-14
    packed |= ((uint32_t)battery & 0x7F) << 15;        // bits 15-21
    packed |= ((uint32_t)left & 0x01) << 22;           // bit 22
    packed |= ((uint32_t)right & 0x01) << 23;          // bit 23

    uint8_t buffer[BUFLEN];
    buffer[0] = (packed >> 0) & 0xFF;
    buffer[1] = (packed >> 8) & 0xFF;
    buffer[2] = (packed >> 16) & 0xFF;

    while (1) {

        int written = uart_write_bytes(UART, (const char*)buffer, BUFLEN);
        if (written == BUFLEN)
        {
            state = !state;
            ESP_ERROR_CHECK(gpio_set_level(GPIO_NUM_4, state));

            vTaskDelay(pdMS_TO_TICKS(500));
        }

        vTaskDelay(pdMS_TO_TICKS(INTERVAL));
    }

}