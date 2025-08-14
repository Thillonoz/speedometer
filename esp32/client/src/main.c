
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>
#include <stdbool.h>

// ===== Testvärden att ändra =====
#define TEST_SPEED_VALUE 100    // 0–255
#define TEST_BATTERY_PERCENT 10 // 0–100
#define TEST_TEMPERATURE_C -20  // -60 till 60
#define TEST_LEFT_INDICATOR 1   // 1 = på, 0 = av
#define TEST_RIGHT_INDICATOR 0  // 1 = på, 0 = av

#define UART_BAUDRATE 1048576
#define SEND_INTERVAL_MS 200 // hur ofta vi skickar

// Konvertera temperatur till 7-bitars two's complement [-60, 60]
static inline uint8_t encodeTemperature7Bit(int temperatureCelsius)
{
    if (temperatureCelsius < -60)
        temperatureCelsius = -60;
    if (temperatureCelsius > 60)
        temperatureCelsius = 60;
    return (uint8_t)(temperatureCelsius & 0x7F);
}

// Packa data till tre bytes enligt Setting::Signal layout
static inline void sendDataFrame(uint8_t speed,
                                 uint8_t batteryPercent,
                                 int temperatureCelsius,
                                 bool leftIndicator,
                                 bool rightIndicator)
{
    if (batteryPercent > 127)
        batteryPercent = 127;

    const uint8_t temp7 = encodeTemperature7Bit(temperatureCelsius);

    // byte0: speed
    const uint8_t byte0 = speed;
    // byte1: temp[6:0] + battery bit0 (MSB)
    const uint8_t byte1 = (uint8_t)((temp7 & 0x7F) | ((batteryPercent & 0x01) << 7));
    // byte2: battery bit1..6 + left(bit6) + right(bit7)
    const uint8_t byte2 = (uint8_t)(((batteryPercent >> 1) & 0x3F) | ((leftIndicator ? 1 : 0) << 6) | ((rightIndicator ? 1 : 0) << 7));

    uint8_t packet[3] = {byte0, byte1, byte2};
    uart_write_bytes(UART_NUM_0, (const char *)packet, sizeof(packet));
}

void app_main(void)
{
    // Initiera UART0 (USB till PC)
    uart_config_t uartConfig = {
        .baud_rate = UART_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_0, &uartConfig);
    uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE,
                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // Skicka testvärden kontinuerligt (vTaskDelay matar watchdog)
    while (1)
    {
        sendDataFrame(TEST_SPEED_VALUE,
                      TEST_BATTERY_PERCENT,
                      TEST_TEMPERATURE_C,
                      TEST_LEFT_INDICATOR,
                      TEST_RIGHT_INDICATOR);

        vTaskDelay(pdMS_TO_TICKS(SEND_INTERVAL_MS));
    }
}