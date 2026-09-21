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

#ifndef _XIAO_BLE_SENSE
#define _XIAO_BLE_SENSE

#define _PINNUM(port, pin)    ((port)*32 + (pin))

/*------------------------------------------------------------------*/
/* LED
 *------------------------------------------------------------------*/
// The board has 3 leds, but changing the number here causes OTA issues.
#define LEDS_NUMBER           1
#define LED_PRIMARY_PIN       _PINNUM(0, 26)
#define LED_STATE_ON          0

#define NEOPIXELS_NUMBER      0

/*------------------------------------------------------------------*/
/* BUTTON
 *------------------------------------------------------------------*/
#define BUTTONS_NUMBER        2
#define BUTTON_1              _PINNUM(0, 18)
// Button 2 is defined as D1 from expansion board.
#define BUTTON_2              _PINNUM(0, 3)
#define BUTTON_PULL           NRF_GPIO_PIN_PULLUP

/*------------------------------------------------------------------*/
/* DISPLAY - optional SSD1306 OLED (user-wired)
 *
 * The XIAO variant offers three I2C pinouts depending on build flags; these
 * are the kit default (NFC pins). On a board wired to D4/D5 or D6/D7 the
 * probe finds nothing and the screen stays blank, which is harmless.
 *------------------------------------------------------------------*/
#define DISPLAY_CONTROLLER_SSD1306
#define DISPLAY_I2C_ADDR      0x3C

#define DISPLAY_PIN_SDA       _PINNUM(0, 30)
#define DISPLAY_PIN_SCL       _PINNUM(0, 31)

#define DISPLAY_WIDTH         128
#define DISPLAY_HEIGHT        64
// Not yet verified on hardware. Offset 0 is right for a true 128-column
// SSD1306; a 132-column SH1106 will sit 2px left of centre, in which case set
// DISPLAY_COL_OFFSET to 2. Blanking spans 132 either way so no uncovered
// column can show power-up garbage.
#define DISPLAY_COL_OFFSET    0
#define DISPLAY_GDDRAM_WIDTH  132

#define DISPLAY_TITLE         "XIAO Sense"

//--------------------------------------------------------------------+
// BLE OTA
//--------------------------------------------------------------------+
#define BLEDIS_MANUFACTURER   "Seeed"
#define BLEDIS_MODEL          "XIAO nRF52840 Sense"

//--------------------------------------------------------------------+
// USB
//--------------------------------------------------------------------+
#define USB_DESC_VID           0x2886
#define USB_DESC_UF2_PID       0x0045
#define USB_DESC_CDC_ONLY_PID  0x0045

//------------- UF2 -------------//
#define UF2_PRODUCT_NAME   "Seeed XIAO nRF52840"
#define UF2_VOLUME_LABEL   "XIAO-SENSE"
#define UF2_BOARD_ID       "nRF52840-SeeedXiaoSense-v1"
#define UF2_INDEX_URL      "https://www.seeedstudio.com/"

#endif // _XIAO_BLE_SENSE
