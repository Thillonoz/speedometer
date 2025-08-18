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

    uint8_t buffer[BUFLEN];
    
    // see settings for bits placement
    buffer[0] = 0b00010100; // 20 (speed)
    buffer[1] = 0b10000001; // 1 and 1 (battery and temperature)
    buffer[2] = 0b11000000; // true and true (light)

    while (1) {
        int written = uart_write_bytes(UART, buffer, BUFLEN);
        if (written == BUFLEN)
        {
            state = !state;
            ESP_ERROR_CHECK(gpio_set_level(GPIO_NUM_4, state));
        }

        vTaskDelay(pdMS_TO_TICKS(INTERVAL));
        fflush(stdout);
    }

}