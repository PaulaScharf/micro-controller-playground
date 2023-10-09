/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "usb_stream.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

#define EXAMPLE_MAX_CHAR_SIZE 64

#define MOUNT_POINT "/sdcard"

// Pin assignments can be set in menuconfig, see "SD SPI Example Configuration" menu.
// You can also change the pin assignments here by changing the following 4 lines.
#define PIN_NUM_MISO 13
#define PIN_NUM_MOSI 11
#define PIN_NUM_CLK 12
#define PIN_NUM_CS 10
#define SD_ENABLE 9

static const char *TAG = "uvc_mic_spk_demo";
/****************** configure the example working mode *******************************/
#define ENABLE_UVC_CAMERA_FUNCTION 1  /* enable uvc function */
#define ENABLE_UAC_MIC_SPK_FUNCTION 0 /* enable uac mic+spk function */
#if (ENABLE_UVC_CAMERA_FUNCTION)
#define ENABLE_UVC_FRAME_RESOLUTION_ANY 1 /* Using any resolution found from the camera */
#define ENABLE_UVC_WIFI_XFER 0            /* transfer uvc frame to wifi http */
#endif
#if (ENABLE_UAC_MIC_SPK_FUNCTION)
#define ENABLE_UAC_MIC_SPK_LOOPBACK 0 /* transfer mic data to speaker */
static uint32_t s_mic_samples_frequence = 0;
static uint32_t s_mic_ch_num = 0;
static uint32_t s_mic_bit_resolution = 0;
static uint32_t s_spk_samples_frequence = 0;
static uint32_t s_spk_ch_num = 0;
static uint32_t s_spk_bit_resolution = 0;
#endif

#define BIT0_FRAME_START (0x01 << 0)
#define BIT1_NEW_FRAME_START (0x01 << 1)
#define BIT2_NEW_FRAME_END (0x01 << 2)
#define BIT3_SPK_START (0x01 << 3)
#define BIT4_SPK_RESET (0x01 << 4)

static EventGroupHandle_t s_evt_handle;

#if (ENABLE_UVC_CAMERA_FUNCTION)
#if (ENABLE_UVC_FRAME_RESOLUTION_ANY)
#define DEMO_UVC_FRAME_WIDTH FRAME_RESOLUTION_ANY
#define DEMO_UVC_FRAME_HEIGHT FRAME_RESOLUTION_ANY
#else
#define DEMO_UVC_FRAME_WIDTH 480
#define DEMO_UVC_FRAME_HEIGHT 320
#endif

#ifdef CONFIG_IDF_TARGET_ESP32S2
#define DEMO_UVC_XFER_BUFFER_SIZE (45 * 1024)
#else
#define DEMO_UVC_XFER_BUFFER_SIZE (55 * 1024)
#endif

#if (ENABLE_UVC_WIFI_XFER)
#include "app_wifi.h"
#include "app_httpd.h"
#include "esp_camera.h"

static camera_fb_t s_fb = {0};

camera_fb_t *esp_camera_fb_get()
{
    xEventGroupSetBits(s_evt_handle, BIT0_FRAME_START);
    xEventGroupWaitBits(s_evt_handle, BIT1_NEW_FRAME_START, true, true, portMAX_DELAY);
    return &s_fb;
}

void esp_camera_fb_return(camera_fb_t *fb)
{
    xEventGroupSetBits(s_evt_handle, BIT2_NEW_FRAME_END);
    return;
}

static void camera_frame_cb(uvc_frame_t *frame, void *ptr)
{
    ESP_LOGI(TAG, "uvc callback! frame_format = %d, seq = %" PRIu32 ", width = %" PRIu32 ", height = %" PRIu32 ", length = %u, ptr = %d",
             frame->frame_format, frame->sequence, frame->width, frame->height, frame->data_bytes, (int)ptr);
    if (!(xEventGroupGetBits(s_evt_handle) & BIT0_FRAME_START))
    {
        return;
    }

    switch (frame->frame_format)
    {
    case UVC_FRAME_FORMAT_MJPEG:
        s_fb.buf = frame->data;
        s_fb.len = frame->data_bytes;
        s_fb.width = frame->width;
        s_fb.height = frame->height;
        s_fb.buf = frame->data;
        s_fb.format = PIXFORMAT_JPEG;
        s_fb.timestamp.tv_sec = frame->sequence;
        xEventGroupSetBits(s_evt_handle, BIT1_NEW_FRAME_START);
        ESP_LOGV(TAG, "send frame = %" PRIu32 "", frame->sequence);
        xEventGroupWaitBits(s_evt_handle, BIT2_NEW_FRAME_END, true, true, portMAX_DELAY);
        ESP_LOGV(TAG, "send frame done = %" PRIu32 "", frame->sequence);
        break;
    default:
        ESP_LOGW(TAG, "Format not supported");
        assert(0);
        break;
    }
}
#else
#include "esp_camera.h"
static camera_fb_t s_fb = {0};
uint8_t counter = 0;
static void camera_frame_cb(uvc_frame_t *frame, void *ptr)
{
    ESP_LOGI(TAG, "uvc callback! frame_format = %d, seq = %" PRIu32 ", width = %" PRIu32 ", height = %" PRIu32 ", length = %u, ptr = %d",
             frame->frame_format, frame->sequence, frame->width, frame->height, frame->data_bytes, (int)ptr);
    switch (frame->frame_format)
    {
    case UVC_FRAME_FORMAT_MJPEG:
        s_fb.buf = frame->data;
        s_fb.len = frame->data_bytes;
        s_fb.width = frame->width;
        s_fb.height = frame->height;
        s_fb.buf = frame->data;
        s_fb.format = PIXFORMAT_JPEG;
        s_fb.timestamp.tv_sec = frame->sequence;
        break;
    default:
        ESP_LOGI(TAG, "sth went wrong :(! frame_format = %d, seq = %" PRIu32 ", width = %" PRIu32 ", height = %" PRIu32 ", length = %u, ptr = %d",
                 frame->frame_format, frame->sequence, frame->width, frame->height, frame->data_bytes, (int)ptr);
        ESP_LOGW(TAG, "Format not supported");
        assert(0);
        break;
    }
    if (counter == 15)
    {
        char photo_name[50];
        sprintf(photo_name, "/sdcard/pic_%lli.jpg", (&s_fb)->timestamp.tv_sec);
        FILE *file = fopen(photo_name, "w");
        if (file == NULL)
        {
            printf("err: fopen failed\n");
        }
        else
        {
            ESP_LOGW(TAG, "wrote file!");
            fwrite((&s_fb)->buf, 1, (&s_fb)->len, file);
            fclose(file);
        }
        counter = 0;
    }
    else
    {
        counter = counter + 1;
    }
}
#endif // ENABLE_UVC_WIFI_XFER
#endif // ENABLE_UVC_CAMERA_FUNCTION

static void stream_state_changed_cb(usb_stream_state_t event, void *arg)
{
    switch (event)
    {
    case STREAM_CONNECTED:
    {
        size_t frame_size = 0;
        size_t frame_index = 0;
#if (ENABLE_UVC_CAMERA_FUNCTION)
        uvc_frame_size_list_get(NULL, &frame_size, &frame_index);
        if (frame_size)
        {
            ESP_LOGI(TAG, "UVC: get frame list size = %u, current = %u", frame_size, frame_index);
            uvc_frame_size_t *uvc_frame_list = (uvc_frame_size_t *)malloc(frame_size * sizeof(uvc_frame_size_t));
            uvc_frame_size_list_get(uvc_frame_list, NULL, NULL);
            for (size_t i = 0; i < frame_size; i++)
            {
                ESP_LOGI(TAG, "\tframe[%u] = %ux%u", i, uvc_frame_list[i].width, uvc_frame_list[i].height);
            }
            free(uvc_frame_list);
        }
        else
        {
            ESP_LOGW(TAG, "UVC: get frame list size = %u", frame_size);
        }
#endif
        ESP_LOGI(TAG, "Device connected");
        break;
    }
    case STREAM_DISCONNECTED:
        ESP_LOGI(TAG, "Device disconnected");
        break;
    default:
        ESP_LOGE(TAG, "Unknown event");
        break;
    }
}

void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("httpd_txrx", ESP_LOG_INFO);
    esp_err_t ret = ESP_FAIL;
    /*********************sd card **********************************/
    gpio_set_direction(SD_ENABLE, GPIO_MODE_OUTPUT);
    gpio_set_level(SD_ENABLE, 0);

    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 45 * 1024};
    sdmmc_card_t *card;
    const char mount_point[] = MOUNT_POINT;
    ESP_LOGI(TAG, "Initializing SD card");

    // Use settings defined above to initialize SD card and mount FAT filesystem.
    // Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
    // Please check its source code and implement error recovery when developing
    // production applications.
    ESP_LOGI(TAG, "Using SPI peripheral");

    // By default, SD card frequency is initialized to SDMMC_FREQ_DEFAULT (20MHz)
    // For setting a specific frequency, use host.max_freq_khz (range 400kHz - 20MHz for SDSPI)
    // Example: for fixed frequency of 10MHz, use host.max_freq_khz = 10000;
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();

    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return;
    }

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;

    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                          "If you want the card to be formatted, set the CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                          "Make sure SD card lines have pull-up resistors in place.",
                     esp_err_to_name(ret));
        }
        return;
    }
    ESP_LOGI(TAG, "Filesystem mounted");

    host.max_freq_khz = 5000;
    host.set_card_clk(host.slot, 5000);

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);
    /*************************************************************************************/
    s_evt_handle = xEventGroupCreate();
    if (s_evt_handle == NULL)
    {
        ESP_LOGE(TAG, "line-%u event group create failed", __LINE__);
        assert(0);
    }

#if (ENABLE_UVC_CAMERA_FUNCTION)
#if (ENABLE_UVC_WIFI_XFER)
    // app_wifi_main();
    // app_httpd_main();
#endif // ENABLE_UVC_WIFI_XFER
    /* malloc double buffer for usb payload, xfer_buffer_size >= frame_buffer_size*/
    uint8_t *xfer_buffer_a = (uint8_t *)malloc(DEMO_UVC_XFER_BUFFER_SIZE);
    assert(xfer_buffer_a != NULL);
    uint8_t *xfer_buffer_b = (uint8_t *)malloc(DEMO_UVC_XFER_BUFFER_SIZE);
    assert(xfer_buffer_b != NULL);

    /* malloc frame buffer for a jpeg frame*/
    uint8_t *frame_buffer = (uint8_t *)malloc(DEMO_UVC_XFER_BUFFER_SIZE);
    assert(frame_buffer != NULL);

    uvc_config_t uvc_config = {
        /* match the any resolution of current camera (first frame size as default) */
        .frame_width = 320,
        .frame_height = 240,
        .frame_interval = FPS2INTERVAL(5),
        .xfer_buffer_size = DEMO_UVC_XFER_BUFFER_SIZE,
        .xfer_buffer_a = xfer_buffer_a,
        .xfer_buffer_b = xfer_buffer_b,
        .frame_buffer_size = DEMO_UVC_XFER_BUFFER_SIZE,
        .frame_buffer = frame_buffer,
        .frame_cb = &camera_frame_cb,
        .frame_cb_arg = NULL,
    };
    /* config to enable uvc function */
    ret = uvc_streaming_config(&uvc_config);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "uvc streaming config failed");
    }
#endif

    /* register the state callback to get connect/disconnect event
     * in the callback, we can get the frame list of current device
     */
    ESP_ERROR_CHECK(usb_streaming_state_register(&stream_state_changed_cb, NULL));
    /* start usb streaming, UVC and UAC MIC will start streaming because SUSPEND_AFTER_START flags not set */
    ESP_ERROR_CHECK(usb_streaming_start());
    ESP_ERROR_CHECK(usb_streaming_connect_wait(portMAX_DELAY));
    // wait for speaker device ready
    xEventGroupWaitBits(s_evt_handle, BIT3_SPK_START, false, false, portMAX_DELAY);

    while (1)
    {
        xEventGroupWaitBits(s_evt_handle, BIT3_SPK_START, true, false, portMAX_DELAY);
        /* Manually resume the speaker because SUSPEND_AFTER_START flags is set */
        usb_streaming_control(STREAM_UAC_SPK, CTRL_UAC_MUTE, (void *)0);
        usb_streaming_control(STREAM_UAC_SPK, CTRL_UAC_VOLUME, (void *)80);
        ESP_ERROR_CHECK(usb_streaming_control(STREAM_UAC_SPK, CTRL_RESUME, NULL));
        ESP_LOGI(TAG, "speaker resume");
    }

    while (1)
    {
        vTaskDelay(100);
    }
}