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

#ifndef _TRACKER_L1
#define _TRACKER_L1

#define _PINNUM(port, pin)    ((port)*32 + (pin))

/*------------------------------------------------------------------*/
/* LED
 *------------------------------------------------------------------*/
#define LEDS_NUMBER           1
#define LED_PRIMARY_PIN       _PINNUM(1, 1)
#define LED_STATE_ON          1

#define NEOPIXELS_NUMBER      0

/*------------------------------------------------------------------*/
/* BUTTON
 *------------------------------------------------------------------*/
#define BUTTONS_NUMBER        2
#define BUTTON_1              _PINNUM(0, 8) // user button
#define BUTTON_2              _PINNUM(0, 8)
#define BUTTON_PULL           NRF_GPIO_PIN_PULLUP

/*------------------------------------------------------------------*/
/* DISPLAY - SSD1306 OLED (variant declares HAS_SCREEN 1, USE_SSD1306 1)
 *------------------------------------------------------------------*/
#define DISPLAY_CONTROLLER_SSD1306
#define DISPLAY_I2C_ADDR      0x3C

#define DISPLAY_PIN_SDA       _PINNUM(0, 9)
#define DISPLAY_PIN_SCL       _PINNUM(0, 10)

#define DISPLAY_WIDTH         128
#define DISPLAY_HEIGHT        64
// Not yet verified on hardware. Offset 0 is right for a true 128-column
// SSD1306; a 132-column SH1106 will sit 2px left of centre, in which case set
// DISPLAY_COL_OFFSET to 2. Blanking spans 132 either way so no uncovered
// column can show power-up garbage.
#define DISPLAY_COL_OFFSET    0
#define DISPLAY_GDDRAM_WIDTH  132

#define DISPLAY_TITLE         "TRACKER L1"

//--------------------------------------------------------------------+
// BLE OTA
//--------------------------------------------------------------------+
#define BLEDIS_MANUFACTURER   "Seeed"
#define BLEDIS_MODEL          "Wio Tracker L1"

//--------------------------------------------------------------------+
// USB
//--------------------------------------------------------------------+
#define USB_DESC_VID           0x2886
#define USB_DESC_UF2_PID       0x1667
#define USB_DESC_CDC_ONLY_PID  0x1667

//------------- UF2 -------------//
#define UF2_PRODUCT_NAME   "Seeed TRACKER L1"
#define UF2_VOLUME_LABEL   "TRACKER L1"
#define UF2_BOARD_ID       "TRACKER L1"
#define UF2_INDEX_URL      "https://www.seeedstudio.com/"

#endif // _TRACKER_L1
