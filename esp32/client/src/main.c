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

#define TAG "CLIENT"
#define DEVICE_NAME "BLE_CLIENT"

#define GATT_SVC_UUID 0xABC0 /* 16 Bit Service UUID */
#define GATT_CHR_UUID 0xABC1 /* 16 Bit Service Characteristic UUID */

static int client_gap_event(struct ble_gap_event *event, void *arg);

static uint8_t own_addr_type;
static uint16_t connection;
static ble_addr_t peer_addr;
static uint16_t chrval_handle;

/* For random static address, 2 MSB bits of the first byte shall be 0b11.
   I.e. addr[5] shall be in the range of 0xC0 to 0xFF */
static const uint8_t server_addr[] = {0x01, 0x04, 0x03, 0x04, 0x05, 0xC0};
static const uint8_t client_addr[] = {0x10, 0x40, 0x30, 0x40, 0x50, 0xC0};

static uint8_t buffer[BUFLEN];

/* ===== Helpers ===== */

static void client_scan(void)
{
    struct ble_gap_disc_params disc_params = {0};

    disc_params.passive = 1;           /* Passive scan */
    disc_params.filter_duplicates = 1; /* De-dup adv reports */

    int status = ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &disc_params, client_gap_event, NULL);
    if (status != 0)
    {
        ESP_LOGE(TAG, "Error initiating GAP discovery; rc=%d", status);
    }
}

static void client_connect(const struct ble_gap_disc_desc *disc)
{
    int status = ble_gap_disc_cancel();
    if (status == 0)
    {
        status = ble_gap_connect(own_addr_type, &disc->addr, 30000, NULL, client_gap_event, NULL);
        if (status != 0)
        {
            char addr_str[18] = {0};
            sprintf(addr_str, "%02X:%02X:%02X:%02X:%02X:%02X",
                    disc->addr.val[5], disc->addr.val[4], disc->addr.val[3],
                    disc->addr.val[2], disc->addr.val[1], disc->addr.val[0]);
            ESP_LOGE(TAG, "Failed to connect; type=%d addr=%s; rc=%d",
                     disc->addr.type, addr_str, status);
        }
    }
    else
    {
        ESP_LOGE(TAG, "Failed to cancel scan; rc=%d", status);
    }
}

/* ===== GATT callbacks ===== */

static int on_read(uint16_t conn_handle,
                   const struct ble_gatt_error *error,
                   struct ble_gatt_attr *attr, void *arg)
{
    if (error->status == 0 && attr && attr->om)
    {
        uint16_t len = OS_MBUF_PKTLEN(attr->om);
        char tmp[256];
        if (len >= sizeof(tmp))
            len = sizeof(tmp) - 1;
        os_mbuf_copydata(attr->om, 0, len, tmp);
        tmp[len] = '\0';
        memcpy(buffer, tmp, BUFLEN);
        //ESP_LOGI(TAG, "READ OK (%u bytes)", len);
        // printf("TMP BYTE: %x %x %x\n", tmp[0], tmp[1], tmp[2]);
        //  printf("BUF BYTE: %x %x %x\n", buffer[0], buffer[1], buffer[2]);
    }
    else if (error->status == BLE_HS_EDONE)
    {
        //ESP_LOGI(TAG, "READ complete");
    }
    else
    {
        //ESP_LOGE(TAG, "READ failed: %d", error->status);
    }
    return 0;
}

static int on_descriptor_discovery(uint16_t conn_handle,
                                   const struct ble_gatt_error *error,
                                   uint16_t chr_val_handle,
                                   const struct ble_gatt_dsc *dsc,
                                   void *arg)
{
    if (error->status == 0 && dsc != NULL)
    {
        if (0 == ble_uuid_cmp(&dsc->uuid.u, BLE_UUID16_DECLARE(BLE_GATT_DSC_CLT_CFG_UUID16)))
        {
            /* Enable notifications: write 0x01 0x00 to CCCD */
            uint8_t value[2] = {1, 0};
            int rc = ble_gattc_write_flat(conn_handle, dsc->handle, value, sizeof(value), NULL, NULL);
            if (rc != 0)
            {
                //ESP_LOGE(TAG, "Failed to write CCCD; rc=%d", rc);
            }
            else
            {
                ESP_LOGI(TAG, "Notifications enabled");
            }
        }
    }
    else if (error->status == BLE_HS_EDONE)
    {
        ESP_LOGI(TAG, "Descriptor discovery complete.");
    }
    else
    {
        ESP_LOGE(TAG, "Descriptor discovery failed: %d", error->status);
        ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
    }
    return 0;
}

static int on_characteristic_discovery(uint16_t conn_handle,
                                       const struct ble_gatt_error *error,
                                       const struct ble_gatt_chr *chr,
                                       void *arg)
{
    if (error->status == 0 && chr != NULL)
    {
        chrval_handle = chr->val_handle;
        ESP_LOGI(TAG, "Characteristic found (handle=%u), discovering descriptors...", chrval_handle);
        int rc = ble_gattc_disc_all_dscs(conn_handle, chr->val_handle, chr->val_handle + 1,
                                         on_descriptor_discovery, NULL);
        if (rc != 0)
        {
            ESP_LOGE(TAG, "Descriptor discovery start failed: %d", rc);
            ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        }
    }
    else if (error->status == BLE_HS_EDONE)
    {
        ESP_LOGI(TAG, "Characteristic discovery complete.");
    }
    else
    {
        ESP_LOGE(TAG, "Characteristic discovery error: %d", error->status);
        ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
    }
    return 0;
}

static int on_service_discovery(uint16_t conn_handle,
                                const struct ble_gatt_error *error,
                                const struct ble_gatt_svc *service,
                                void *arg)
{
    if (error->status == 0 && service != NULL)
    {
        uint16_t svc_start_handle = service->start_handle;
        uint16_t svc_end_handle = service->end_handle;
        ESP_LOGI(TAG, "Service found [%u..%u], discovering characteristic...", svc_start_handle, svc_end_handle);
        int rc = ble_gattc_disc_chrs_by_uuid(conn_handle, svc_start_handle, svc_end_handle,
                                             BLE_UUID16_DECLARE(GATT_CHR_UUID),
                                             on_characteristic_discovery, NULL);
        if (rc != 0)
        {
            ESP_LOGE(TAG, "Char discovery start failed: %d", rc);
            ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        }
    }
    else if (error->status == BLE_HS_EDONE)
    {
        ESP_LOGI(TAG, "Service discovery complete.");
    }
    else
    {
        ESP_LOGE(TAG, "Service discovery failed; status=%d", error->status);
        ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
    }
    return 0;
}

/* ===== GAP event handler ===== */

static int client_gap_event(struct ble_gap_event *event, void *arg)
{
    (void)arg;

    int status = 0;
    struct ble_gap_conn_desc desc;
    struct ble_hs_adv_fields fields;

    switch (event->type)
    {
    case BLE_GAP_EVENT_DISC:
    {
        status = ble_hs_adv_parse_fields(&fields, event->disc.data, event->disc.length_data);
        if (status == 0)
        {
            bool connected = false;

            if (0 == memcmp(peer_addr.val, event->disc.addr.val, sizeof(event->disc.addr.val)))
            {
                ESP_LOGI(TAG, "Device already connected");
                connected = true;
                break;
            }

            if (!connected)
            {
                /* Only consider connectable adv/dir_ind */
                if ((event->disc.event_type == BLE_HCI_ADV_RPT_EVTYPE_ADV_IND) ||
                    (event->disc.event_type == BLE_HCI_ADV_RPT_EVTYPE_DIR_IND))
                {

                    if (0 == memcmp(event->disc.addr.val, server_addr, sizeof(server_addr)))
                    {
                        /* Require that advertised UUIDs include our service */
                        for (int i = 0; i < fields.num_uuids16; i++)
                        {
                            if (ble_uuid_u16(&fields.uuids16[i].u) == GATT_SVC_UUID)
                            {
                                client_connect(&event->disc);
                                break;
                            }
                        }
                    }
                }
            }
        }
        break;
    }

    case BLE_GAP_EVENT_CONNECT:
        if (event->connect.status == 0)
        {
            ESP_LOGI(TAG, "Connection established");
            assert(0 == ble_gap_conn_find(event->connect.conn_handle, &desc));
            connection = event->connect.conn_handle;
            memcpy(peer_addr.val, desc.peer_id_addr.val, sizeof(desc.peer_id_addr.val));

            int rc = ble_gattc_disc_svc_by_uuid(connection,
                                                BLE_UUID16_DECLARE(GATT_SVC_UUID),
                                                on_service_discovery, NULL);
            if (rc != 0)
            {
                ESP_LOGE(TAG, "Service discovery start failed: %d", rc);
                ble_gap_terminate(connection, BLE_ERR_REM_USER_CONN_TERM);
            }
        }
        else
        {
            ESP_LOGE(TAG, "Connection failed; status=%d", event->connect.status);
            client_scan();
        }
        break;

    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI(TAG, "Disconnected; reason=%d", event->disconnect.reason);
        memset(peer_addr.val, 0, sizeof(peer_addr.val));
        chrval_handle = 0;
        client_scan();
        break;

    case BLE_GAP_EVENT_DISC_COMPLETE:
        ESP_LOGI(TAG, "Discovery complete; reason=%d", event->disc_complete.reason);
        break;

    case BLE_GAP_EVENT_NOTIFY_RX:
    {
        ESP_LOGI(TAG, "Received %s; conn=%d attr_handle=%d len=%d",
                 event->notify_rx.indication ? "indication" : "notification",
                 event->notify_rx.conn_handle, event->notify_rx.attr_handle,
                 OS_MBUF_PKTLEN(event->notify_rx.om));

        uint16_t len = OS_MBUF_PKTLEN(event->notify_rx.om);
        char buf[256];
        if (len >= sizeof(buf))
            len = sizeof(buf) - 1;
        os_mbuf_copydata(event->notify_rx.om, 0, len, buf);
        buf[len] = '\0';
        // printf("Notify: %s\n", buf);
        break;
    }

    case BLE_GAP_EVENT_MTU:
        ESP_LOGI(TAG, "MTU update; conn=%d cid=%d mtu=%d",
                 event->mtu.conn_handle, event->mtu.channel_id, event->mtu.value);
        break;

    default:
        break;
    }

    return status;
}

/* ===== Host callbacks ===== */
static void client_on_reset(int reason)
{
    ESP_LOGE(TAG, "Resetting state; reason=%d", reason);
}

static void client_on_sync(void)
{
    assert(0 == ble_hs_id_set_rnd(client_addr)); /* random static */
    assert(0 == ble_hs_util_ensure_addr(0));
    assert(0 == ble_hs_id_infer_auto(0, &own_addr_type));

    uint8_t addr[sizeof(server_addr)] = {0};
    assert(0 == ble_hs_id_copy_addr(own_addr_type, addr, NULL));

    // printf("BLE Device Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
    //        addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);

    client_scan();
}

/* ===== App task ===== */

void client_task(void *pvParameters)
{
    (void)pvParameters;

    while (1)
    {
        if (chrval_handle != 0)
        {
            /* Actively read the characteristic to confirm data path.
               Result is logged in on_read(). */
            int rc = ble_gattc_read(connection, chrval_handle, on_read, NULL);
            if (rc != 0)
            {
                //ESP_LOGE(TAG, "ble_gattc_read failed: %d", rc);
            }
        }

        //printf("UART BYTE: %x %x %x\n", buffer[0], buffer[1], buffer[2]);
        uart_write_bytes(UART, buffer, BUFLEN);
        //printf("written: %d\n", written);

        fflush(stdout);
        vTaskDelay(pdMS_TO_TICKS(INTERVAL / 2));
    }
}

/* ===== Entry ===== */
void app_main(void)
{

    // TEMP LED to test
    ESP_ERROR_CHECK(gpio_reset_pin(GPIO_NUM_4));

    ESP_ERROR_CHECK(gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT)); // Configure pin 4 as a digital output pin

    // UART config
    uart_config_t uart_config = {
        .baud_rate = BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_driver_install(UART, 256, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART, &uart_config));
    // (Optional) Set pins if needed: uart_set_pin(UART, TX, RX, RTS, CTS);

    // see settings for bits placement
    // buffer[0] = 0b00010100; // 20 (speed)
    // buffer[1] = 0b10000010; // 1 and 1 (battery and temperature)
    // buffer[2] = 0b11000000; // true and true (light)

    esp_err_t status = nvs_flash_init();
    if (status == ESP_ERR_NVS_NO_FREE_PAGES || status == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        status = nvs_flash_init();
    }
    ESP_ERROR_CHECK(status);

    ESP_ERROR_CHECK(nimble_port_init());

    /* Configure the host. */
    ble_hs_cfg.reset_cb = client_on_reset;
    ble_hs_cfg.sync_cb = client_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    /* Set the default device name. */
    assert(0 == ble_svc_gap_device_name_set(DEVICE_NAME));

    /* Periodic task that triggers reads (and logs notifications when they arrive). */
    assert(pdTRUE == xTaskCreate(client_task, "client_task", 4096, NULL, 8, NULL));

    // printf("START\n");
    //ESP_LOGI(TAG, "BLE Host Task Started");
    nimble_port_run(); /* Returns only when nimble_port_stop() is called */
    nimble_port_freertos_deinit();
}