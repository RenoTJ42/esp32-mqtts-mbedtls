/**
 *
 * Architecture:
 *   - Sensor Task  (Core 0): BME280 poll → FreeRTOS queue
 *   - MQTT Task    (Core 1): Queue consumer → TLS publish
 *   - WDT Task     (Core 0): System health → UART diagnostics
 *
 * Phase 1: Partition table verification only.
 *          UART, GPIO, I2C, TLS added in subsequent phases.
 */

#include <stdio.h>
#include "esp_log.h"
#include "esp_partition.h"
#include "esp_idf_version.h"

#include "uart_driver.h"
#include "gpio_driver.h"

static const char *TAG = "MAIN";

/**
 * Verify our custom 'certs' partition exists and is accessible.
 *        This proves the partition table was flashed correctly.
 */
static void verify_partitions(void)
{
    ESP_LOGI(TAG, "--- Partition Table Verification ---");

    /* Iterate every partition and log it */
    esp_partition_iterator_t it = esp_partition_find(
                                      ESP_PARTITION_TYPE_ANY,
                                      ESP_PARTITION_SUBTYPE_ANY,
                                      NULL);

    while (it != NULL) {
        const esp_partition_t *p = esp_partition_get(it);
        ESP_LOGI(TAG, "  [%-12s] type=0x%02x sub=0x%02x "
                      "offset=0x%08"PRIx32" size=0x%08"PRIx32,
                 p->label, p->type, p->subtype,
                 p->address, p->size);
        it = esp_partition_next(it);
    }
    esp_partition_iterator_release(it);

    /* Now specifically find OUR certs partition */
    const esp_partition_t *certs = esp_partition_find_first(
                                       ESP_PARTITION_TYPE_DATA,
                                       0xFF,   /* our custom subtype */
                                       "certs");

    if (certs == NULL) {
        ESP_LOGE(TAG, "FATAL: 'certs' partition NOT FOUND. "
                      "Check partitions.csv and re-flash.");
    } else {
        ESP_LOGI(TAG, "OK: 'certs' partition found at "
                      "offset=0x%08"PRIx32" size=0x%08"PRIx32,
                 certs->address, certs->size);
    }

    ESP_LOGI(TAG, "------------------------------------");
}

void app_main(void)
{
    uart_driver_init();
    uart_write_str("\r\n=== Secure IoT ESP32 — UART Driver Online ===\r\n");
    gpio_driver_init();
    gpio_led_blink(1000);

    ESP_LOGI(TAG, "Phase 1 Boot...");
    
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, " Secure IoT ESP32 — Phase 1 Boot");
    ESP_LOGI(TAG, " ESP-IDF v%s", esp_get_idf_version());
    ESP_LOGI(TAG, "========================================");

    verify_partitions();

    /*
     * Phase 2+: Task creation goes here.
     * xTaskCreatePinnedToCore(sensor_task, ...);
     * xTaskCreatePinnedToCore(mqtt_task,   ...);
     * xTaskCreatePinnedToCore(wdt_task,    ...);
     */

    ESP_LOGI(TAG, "Phase 1 complete. System idle.");
}
