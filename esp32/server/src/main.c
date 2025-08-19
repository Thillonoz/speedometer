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

#define UART_BLE_TESTING // GPIO testing, comment out to disable
#ifdef UART_BLE_TESTING

#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <unistd.h>

#define LED_GPIO4 GPIO_NUM_4
#define LED_GPIO5 GPIO_NUM_5

void tempstart(void)
{
    static const char *TAG = "LED_CONTROL";

    ESP_LOGI(TAG, "Configuring GPIO...");

    gpio_reset_pin(LED_GPIO4);
    gpio_config_t io_conf = {.pin_bit_mask = (1ULL << LED_GPIO4),
                             .mode = GPIO_MODE_OUTPUT,
                             .pull_up_en = GPIO_PULLUP_DISABLE,
                             .pull_down_en = GPIO_PULLDOWN_DISABLE,
                             .intr_type = GPIO_INTR_DISABLE};

    ESP_ERROR_CHECK(gpio_config(&io_conf));

    gpio_reset_pin(LED_GPIO5);
    gpio_config_t io_conf_2 = {.pin_bit_mask = (1ULL << LED_GPIO5),
                             .mode = GPIO_MODE_OUTPUT,
                             .pull_up_en = GPIO_PULLUP_DISABLE,
                             .pull_down_en = GPIO_PULLDOWN_DISABLE,
                             .intr_type = GPIO_INTR_DISABLE};

    ESP_ERROR_CHECK(gpio_config(&io_conf_2));
}

void tempCheck(uint8_t *_buffer, uint8_t _bool)
{
    if (_bool == true)
    {
        if (_buffer[0] == 0b00000001 || _buffer[1] == 0b00000001 || _buffer[1] == 0b10000000 ||
          _buffer[2] == 0b01000000 || _buffer[2] == 0b10000000 || _buffer[2] == 0b11000000) {
          gpio_set_level(LED_GPIO4, 1);
        }
        else
        {
            gpio_set_level(LED_GPIO4, 0);
        }
    }
    else
    {
        gpio_set_level(LED_GPIO4, 1);
    }
}
#endif

#define UART UART_NUM_0
#define BUF_SIZE (2 * SOC_UART_FIFO_LEN) // SOC_UART_FIFO_LEN
#define MSGLEN BUFLEN

// #include <ctype.h>

#define TAG "SERVER_LOGGER"
#define DEVICE_NAME "BLE_SERVER"

#define BLE_SVC_UUID16 0xABC0     /* 16 Bit Service UUID */
#define BLE_SVC_CHR_UUID16 0xABC1 /* 16 Bit Service Characteristic UUID */

static int server_gap_event(struct ble_gap_event *event, void *arg);
static int service_gatt_handler(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg);

static uint8_t own_addr_type;
static uint16_t ble_svc_gatt_read_val_handle;
static uint16_t conn_handle_global = BLE_HS_CONN_HANDLE_NONE;

// For random static address, 2 MSB bits of the first byte shall be 0b11.
// I.e. addr[5] shall be in the range of 0xC0 to 0xFF
static const uint8_t server_addr[] = {0x01, 0x04, 0x03, 0x04, 0x05, 0xC0};
static const uint8_t client_addr[] = {0x10, 0x40, 0x30, 0x40, 0x50, 0xC0};

// Setting::INTERVAL, change when setting is updated.
static const int INTERVAL = 40;
static char buffer[MSGLEN];

static const struct ble_gatt_svc_def new_ble_svc_gatt_defs[] = {
    {
        /* The Service */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BLE_SVC_UUID16),
        .characteristics =
            (struct ble_gatt_chr_def[]){
                {
                    /* The characteristic */
                    .uuid = BLE_UUID16_DECLARE(BLE_SVC_CHR_UUID16),
                    .access_cb = service_gatt_handler,
                    .val_handle = &ble_svc_gatt_read_val_handle,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                },
                {
                    0,
                },
            },
    },
    {
        0,
    },
};

// Logs information about a connection to the console.
static void server_print_conn_desc(const struct ble_gap_conn_desc *desc)
{
    char addr[18];

    sprintf(addr, "%02x:%02x:%02x:%02x:%02x:%02x", desc->our_id_addr.val[5],
            desc->our_id_addr.val[4], desc->our_id_addr.val[3],
            desc->our_id_addr.val[2], desc->our_id_addr.val[1],
            desc->our_id_addr.val[0]);
    ESP_LOGI(TAG, " our_id_addr_type=%d our_id_addr=%s", desc->our_id_addr.type,
             addr);

    sprintf(addr, "%02x:%02x:%02x:%02x:%02x:%02x", desc->peer_id_addr.val[5],
            desc->peer_id_addr.val[4], desc->peer_id_addr.val[3],
            desc->peer_id_addr.val[2], desc->peer_id_addr.val[1],
            desc->peer_id_addr.val[0]);
    ESP_LOGI(TAG, " peer_id_addr_type=%d peer_id_addr=%s",
             desc->peer_id_addr.type, addr);

    ESP_LOGI(TAG,
             " conn_itvl=%d conn_latency=%d supervision_timeout=%d encrypted=%d "
             "authenticated=%d bonded=%d\n",
             desc->conn_itvl, desc->conn_latency, desc->supervision_timeout,
             desc->sec_state.encrypted, desc->sec_state.authenticated,
             desc->sec_state.bonded);
}

static void server_advertise(void)
{
    struct ble_hs_adv_fields fields = {0};
    const char *name = ble_svc_gap_device_name();

    // General discoverability and BLE-only (BR/EDR unsupported)
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    /* Set device name */
    fields.name = (uint8_t *)name;
    fields.name_len = strlen(name);
    fields.name_is_complete = 1;

    /* Set device tx power */
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;
    fields.tx_pwr_lvl_is_present = 1;

    /* 16-bit service UUIDs (alert notifications) */
    fields.uuids16 = (ble_uuid16_t[]){BLE_UUID16_INIT(BLE_SVC_UUID16)};
    fields.uuids16_is_complete = 1;
    fields.num_uuids16 = 1;

    /* Set device LE role */
    fields.le_role = BLE_GAP_ROLE_SLAVE;
    fields.le_role_is_present = 1;

    int status = ble_gap_adv_set_fields(&fields);
    if (status == 0)
    {
        struct ble_gap_adv_params adv_params = {0};

        /* Set connetable and general discoverable mode */
        adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
        adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
        adv_params.filter_policy = BLE_HCI_ADV_FILT_BOTH;

        /* Start advertising */
        status = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER, &adv_params,
                                   server_gap_event, NULL);
        if (status == 0)
        {
            ESP_LOGI(TAG, "Advertising started!");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to start advertising, error code: %d", status);
        }
    }
    else
    {
        ESP_LOGE(TAG, "Error setting advertisement data; status = %d\n", status);
    }
}

static int server_gap_event(struct ble_gap_event *event, void *)
{
    struct ble_gap_conn_desc desc;

    switch (event->type)
    {
    case BLE_GAP_EVENT_CONNECT: /* A new connection was established or a
                                   connection attempt failed. */
        ESP_LOGI(TAG, "connection %s; status=%d ",
                 event->connect.status == 0 ? "established" : "failed",
                 event->connect.status);
        if (event->connect.status == 0)
        {
            conn_handle_global = event->connect.conn_handle;
            assert(0 == ble_gap_conn_find(event->connect.conn_handle, &desc));
            server_print_conn_desc(&desc);
        }
        else
        {
            /* Connection failed; resume advertising. */
            server_advertise();
            conn_handle_global = BLE_HS_CONN_HANDLE_NONE;
        }
        break;

    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI(TAG, "disconnect; reason=%d ", event->disconnect.reason);
        server_print_conn_desc(&event->disconnect.conn);
        server_advertise(); /* Connection terminated; resume advertising. */
        conn_handle_global = BLE_HS_CONN_HANDLE_NONE;
        break;

    case BLE_GAP_EVENT_CONN_UPDATE: /* The central has updated the connection
                                       parameters. */
        ESP_LOGI(TAG, "connection updated; status=%d ", event->conn_update.status);
        assert(0 == ble_gap_conn_find(event->conn_update.conn_handle, &desc));
        server_print_conn_desc(&desc);
        break;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI(TAG, "advertise complete; reason=%d", event->adv_complete.reason);
        server_advertise();
        break;

    case BLE_GAP_EVENT_MTU:
        /* Maximum Transmission Unit defines the maximum size of a single ATT
         (Attribute Protocol) payload, i.e., how much data can be sent in a single
         BLE GATT read/write/notify/indication operation. */
        ESP_LOGI(TAG, "MTU update event; conn_handle=%d cid=%d mtu=%d\n",
                 event->mtu.conn_handle, event->mtu.channel_id, event->mtu.value);
        break;

    case BLE_GAP_EVENT_SUBSCRIBE:
        ESP_LOGI(TAG,
                 "subscribe event; conn_handle=%d attr_handle=%d  reason=%d "
                 "prevn=%d curn=%d previ=%d curi=%d\n",
                 event->subscribe.conn_handle, event->subscribe.attr_handle,
                 event->subscribe.reason, event->subscribe.prev_notify,
                 event->subscribe.cur_notify, event->subscribe.prev_indicate,
                 event->subscribe.cur_indicate);
        break;

    default:
        break;
    }

    return 0;
}

static void server_on_reset(int reason)
{
    ESP_LOGE(TAG, "Resetting state; reason=%d\n", reason);
}

static void server_on_sync(void)
{
    assert(0 == ble_hs_id_set_rnd(server_addr));

    assert(0 == ble_hs_util_ensure_addr(0));

    assert(0 == ble_hs_id_infer_auto(0, &own_addr_type));

    uint8_t addr_val[6] = {0};
    assert(0 == ble_hs_id_copy_addr(own_addr_type, addr_val, NULL));

    printf("BLE Device Address: %02X:%02X:%02X:%02X:%02X:%02X\n", addr_val[5],
           addr_val[4], addr_val[3], addr_val[2], addr_val[1], addr_val[0]);

    ble_addr_t client = {.type = BLE_ADDR_RANDOM};
    memcpy(client.val, client_addr, sizeof(client_addr));

    assert(0 == ble_gap_wl_set(&client, 1));

    server_advertise();
}

/* Callback function for custom service */
static int service_gatt_handler(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *)
{
    switch (ctxt->op)
    {
    case BLE_GATT_ACCESS_OP_READ_CHR:
    {
        ESP_LOGI(TAG, "Callback for read");
#ifdef UART_BLE_TESTING
        gpio_set_level(LED_GPIO5, 1);
#endif
        fflush(stdout);
        int rc = os_mbuf_append(ctxt->om, buffer, sizeof(buffer));
        if (rc != 0)
        {
            ESP_LOGE(TAG, "Error appending data to mbuf");
            // return BLE_ATT_ERR_INSUFFICIENT_RES;
        }
#ifdef UART_BLE_TESTING
        gpio_set_level(LED_GPIO5, 0);
#endif
    }

    break;

    default:
        ESP_LOGI(TAG, "\nDefault Callback");
        break;
    }

    return 0;
}

static void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *)
{
    char buf[BLE_UUID_STR_LEN] = {0};

    switch (ctxt->op)
    {
    case BLE_GATT_REGISTER_OP_SVC:
        ESP_LOGI(TAG, "registered service %s with handle=%d\n",
                 ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf), ctxt->svc.handle);
        break;

    case BLE_GATT_REGISTER_OP_CHR:
        ESP_LOGI(TAG,
                 "registering characteristic %s with def_handle=%d val_handle=%d\n",
                 ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                 ctxt->chr.def_handle, ctxt->chr.val_handle);
        break;

    case BLE_GATT_REGISTER_OP_DSC:
        ESP_LOGI(TAG, "registering descriptor %s with handle=%d\n",
                 ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf), ctxt->dsc.handle);
        break;

    default:
        assert(0);
        break;
    }
}

int gatt_svr_init(void)
{
    ble_svc_gap_init();
    ble_svc_gatt_init();

    int status = ble_gatts_count_cfg(new_ble_svc_gatt_defs);

    if (status == 0)
    {
        status = ble_gatts_add_svcs(new_ble_svc_gatt_defs);
    }

    return status;
}

void uart_ble_task(void *arg)
{
    while (1)
    {
        fflush(stdout);
        int len = uart_read_bytes(UART, buffer, MSGLEN, pdMS_TO_TICKS(INTERVAL));
        if (len == MSGLEN)
        {
#ifdef UART_BLE_TESTING
            tempCheck((uint8_t *)buffer, true);
#endif
            struct os_mbuf *om = ble_hs_mbuf_from_flat(buffer, len);
            ble_gatts_notify_custom(conn_handle_global, ble_svc_gatt_read_val_handle, om);
        }
        else
        {
#ifdef UART_BLE_TESTING
            tempCheck((uint8_t *)buffer, false);
#endif
            ESP_LOGE(TAG, "Error in receiving data from UART");

            buffer[0] = 0x69; // Default value if no data received
            buffer[1] = 0x89; // Default value if no data received
            buffer[2] = 0xd9; // Default value if no data received
        }
        vTaskDelay(pdMS_TO_TICKS(INTERVAL));
    }
}

void app_main()
{
    tempstart();

    // UART configuration
    uart_config_t config = {
        .baud_rate = BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_driver_install(UART, BUF_SIZE, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART, &config));

    // BLE configuration
    esp_err_t status = nvs_flash_init();
    if (status == ESP_ERR_NVS_NO_FREE_PAGES ||
        status == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        status = nvs_flash_init();
    }
    ESP_ERROR_CHECK(status);

    ESP_ERROR_CHECK(nimble_port_init());
    ESP_ERROR_CHECK(esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P20));

    ble_hs_cfg.reset_cb = server_on_reset;
    ble_hs_cfg.sync_cb = server_on_sync;
    ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    assert(0 == gatt_svr_init());
    assert(0 == ble_svc_gap_device_name_set(DEVICE_NAME));

    xTaskCreate(uart_ble_task, "uart_ble_task", 4096, NULL, 5, NULL);

    ESP_LOGI(TAG, "BLE Host Task Started");
    nimble_port_run();
    nimble_port_freertos_deinit();
}