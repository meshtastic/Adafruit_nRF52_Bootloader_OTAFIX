/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Ha Thach for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "nrfx.h"
#include "nrfx_power.h"
#include "nrf_sdm.h"
#include "nrf_soc.h"

#include "app_timer.h"
#include "bootloader.h"

#include "nrf_usbd.h"
#include "tusb.h"
#include "usb_desc.h"

#include "uf2/uf2.h"
#include "boards.h"

//--------------------------------------------------------------------+
// MACRO TYPEDEF CONSTANT ENUM DECLARATION
//--------------------------------------------------------------------+

/* tinyusb function that handles power event (detected, ready, removed)
 * We must call it within SD's SOC event handler, or set it as power event handler if SD is not enabled. */
extern void tusb_hal_nrf_power_event(uint32_t event);

// power callback when SD is not enabled
static void power_event_handler(nrfx_power_usb_evt_t event) {
  tusb_hal_nrf_power_event((uint32_t) event);
}

// Forward USB interrupt events to TinyUSB IRQ Handler
void USBD_IRQHandler(void) {
  tud_int_handler(0);
}

//------------- IMPLEMENTATION -------------//
void usb_init(bool cdc_only) {
  // 0, 1 is reserved for SD
  NVIC_SetPriority(USBD_IRQn, 2);

  // USB power may already be ready at this time -> no event generated
  // We need to invoke the handler based on the status initially
  uint32_t usb_reg;
  uint8_t sd_en = false;

  if (is_sd_existed()) {
    sd_softdevice_is_enabled(&sd_en);
  }

  if (sd_en) {
    sd_power_usbdetected_enable(true);
    sd_power_usbpwrrdy_enable(true);
    sd_power_usbremoved_enable(true);
    sd_power_usbregstatus_get(&usb_reg);
  } else {
    // Power module init
    const nrfx_power_config_t pwr_cfg = {0};
    nrfx_power_init(&pwr_cfg);

    // Register USB power handler
    const nrfx_power_usbevt_config_t config = {.handler = power_event_handler};
    nrfx_power_usbevt_init(&config);

    nrfx_power_usbevt_enable();

    usb_reg = NRF_POWER->USBREGSTATUS;
  }

  if (usb_reg & POWER_USBREGSTATUS_VBUSDETECT_Msk) {
    tusb_hal_nrf_power_event(NRFX_POWER_USB_EVT_DETECTED);
  }

  if (usb_reg & POWER_USBREGSTATUS_OUTPUTRDY_Msk) {
    tusb_hal_nrf_power_event(NRFX_POWER_USB_EVT_READY);
  }

  usb_desc_init(cdc_only);
  uf2_init();
  tusb_init();

  #ifdef BOARD_HAS_DISPLAY
  if (board_display_init()) {
    screen_draw_drag();
  }
  #endif
}

void usb_teardown(void) {
  // Simulate an disconnect which cause pullup disable, USB perpheral disable and hclk disable
  tusb_hal_nrf_power_event(NRFX_POWER_USB_EVT_REMOVED);
}

//--------------------------------------------------------------------+
// tinyusb callbacks
//--------------------------------------------------------------------+
static bool _host_mounted;

bool usb_host_mounted(void) {
  return _host_mounted;
}

void tud_mount_cb(void) {
  _host_mounted = true;
  dfu_activity();
  led_state(STATE_USB_MOUNTED);
}

// Cable pulled with no app to boot: end the session so main resets to the BLE OTA wait.
#define VBUS_LOSS_DELAY_MS 250
APP_TIMER_DEF(_vbus_loss_timer);
static bool _vbus_loss_timer_created;

static bool vbus_lost_without_app(void) {
  return !(NRF_POWER->USBREGSTATUS & POWER_USBREGSTATUS_VBUSDETECT_Msk) && !bootloader_app_is_valid();
}

static void vbus_loss_timer_handler(void *p_context) {
  (void) p_context;
  if (vbus_lost_without_app()) {
    dfu_update_status_t update_status = { .status_code = DFU_RESET };
    bootloader_dfu_update_process(update_status);
  }
}

void tud_umount_cb(void) {
  led_state(STATE_USB_UNMOUNTED);

  if (vbus_lost_without_app()) {
    if (!_vbus_loss_timer_created) {
      app_timer_create(&_vbus_loss_timer, APP_TIMER_MODE_SINGLE_SHOT, vbus_loss_timer_handler);
      _vbus_loss_timer_created = true;
    }
    app_timer_stop(_vbus_loss_timer);
    app_timer_start(_vbus_loss_timer, APP_TIMER_TICKS(VBUS_LOSS_DELAY_MS), NULL);
  }
}
