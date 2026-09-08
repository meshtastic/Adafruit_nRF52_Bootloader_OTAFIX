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

#ifndef _HELTEC_T096_H
#define _HELTEC_T096_H

#define _PINNUM(port, pin)    ((port)*32 + (pin))

/*------------------------------------------------------------------*/
/* LED
 *------------------------------------------------------------------*/
#define LEDS_NUMBER           1
#define LED_PRIMARY_PIN       _PINNUM(0, 28) // White
#define LED_STATE_ON          0

// #define LED_NEOPIXEL          _PINNUM(0, 14)
// #define NEOPIXEL_POWER_PIN    _PINNUM(0, 21)
// #define NEOPIXELS_NUMBER      2
#define BOARD_RGB_BRIGHTNESS  0x040404

/*------------------------------------------------------------------*/
/* BUTTON
 *------------------------------------------------------------------*/
#define BUTTONS_NUMBER        2
#define BUTTON_1              _PINNUM(1, 10)
#define BUTTON_2              _PINNUM(1, 2) // stock bootloader has this set to 34
#define BUTTON_PULL           NRF_GPIO_PIN_PULLUP

//--------------------------------------------------------------------+
// Display
//--------------------------------------------------------------------+

// VSensor required to power the display
#define DISPLAY_VSENSOR_PIN   _PINNUM(0,  26)
#define DISPLAY_VSENSOR_ON    1 

// #define DISPLAY_CONTROLLER_ST7789
#define DISPLAY_CONTROLLER_ST7735

#define DISPLAY_PIN_SCK       _PINNUM(0, 20)
#define DISPLAY_PIN_MOSI      _PINNUM(0, 17)

#define DISPLAY_PIN_CS        _PINNUM(0, 22)
#define DISPLAY_PIN_DC        _PINNUM(0, 15)
#define DISPLAY_PIN_RST       _PINNUM(0, 13)
#define DISPLAY_PIN_BL        _PINNUM(1, 12)
#define DISPLAY_BL_ON         0  // GPIO state to enable back light

#define DISPLAY_WIDTH         160
#define DISPLAY_HEIGHT        80

// USB FACING LEFT
#define DISPLAY_COL_OFFSET    24
#define DISPLAY_ROW_OFFSET    0
#define DISPLAY_MADCTL        (ST77XX_MADCTL_MY | ST7735_MADCTL_BGR)

// USB FACING RIGHT
//#define DISPLAY_COL_OFFSET 52
//#define DISPLAY_ROW_OFFSET 40
//#define DISPLAY_MADCTL   (ST77XX_MADCTL_MX | ST7735_MADCTL_BGR)

#define DISPLAY_VSCSAD        0

#define DISPLAY_TITLE         "T096"
#define BANNER_TEXT           "meshtastic.org - OTAFIX"
#define FONT_SIZE_LARGE       3
#define NOLABELS

#define SCREEN_BAR1_Y         0
#define SCREEN_BAR1_H         32
#define SCREEN_BAR2_Y         32
#define SCREEN_BAR2_H         36
#define SCREEN_BAR3_Y         68
#define SCREEN_BAR3_H         12
#define BANNER_TEXT_X         0
#define BANNER_TEXT_Y         70
#define DISPLAY_TITLE_Y       1
#define UF2_VERSION_BASE_Y    24
#define BLE_OTA_Y             38
#define DRAG 34
#define DRAGX 4
// Three 32px icons have to fit in 160px: the default pendriveLogo_X of
// 129 puts the last one at x=133..164, off the right edge of the panel.
#define pendriveLogo_X        124

//--------------------------------------------------------------------+
// BLE OTA
//--------------------------------------------------------------------+
#define BLEDIS_MANUFACTURER   "Heltec AutoMation"
#define BLEDIS_MODEL          "Tracker"

//--------------------------------------------------------------------+
// USB
//--------------------------------------------------------------------+
#define USB_DESC_VID           0x239A
#define USB_DESC_UF2_PID       0x0071
#define USB_DESC_CDC_ONLY_PID  0x0071

//------------- UF2 -------------//
#define UF2_PRODUCT_NAME      "Heltec-T096"
#define UF2_VOLUME_LABEL      "HeltecT096"
#define UF2_BOARD_ID          "Heltec-T096-v1"
#define UF2_INDEX_URL         "http://heltec.org/"

#endif
