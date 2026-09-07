/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Yihui Xiong for Makerdiary
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

#ifndef _MESH_TRACKER_X1_H
#define _MESH_TRACKER_X1_H

#define _PINNUM(port, pin)    ((port)*32 + (pin))

/*------------------------------------------------------------------*/
/* LED
 *------------------------------------------------------------------*/
/* The X1 carries an RGB LED, but it belongs to the application firmware and
 * the bootloader drives no status LED of its own, so LEDS_NUMBER is 0. The
 * pins are recorded for reference; wiring them up as this repo's RGB status
 * LED (LED_RGB_RED_PIN / _GREEN_PIN / _BLUE_PIN, which PWM-drive the DFU
 * state colours) needs the LED's polarity confirmed on real hardware
 * first. */
#define LEDS_NUMBER           0
#define LED_RED_PIN           _PINNUM(0, 3)   // Red
#define LED_GREEN_PIN         _PINNUM(0, 24)  // Green
#define LED_BLUE_PIN          _PINNUM(0, 28)  // Blue
#define LED_STATE_ON          1

/*------------------------------------------------------------------*/
/* BUTTON
 *------------------------------------------------------------------*/
/* P0.18 is RESET and is not usable as a DFU button, so double-reset entry is
 * unavailable and the primary button is the only way into DFU. A momentary
 * press belongs to the application, so this board opts into the hold-to-enter
 * scheme instead (see BUTTON_DFU_HOLD in src/main.c). BUTTON_2 is declared
 * only to satisfy the BUTTONS_NUMBER >= 2 check in boards.h. */
#define BUTTONS_NUMBER        2
#define BUTTON_1              _PINNUM(0, 6)  // Primary Button
#define BUTTON_2              _PINNUM(0, 18) // unusable: RESET
#define BUTTON_PULL           NRF_GPIO_PIN_PULLDOWN

// Hold the primary button through boot for this long to force UF2 DFU.
#define BUTTON_DFU_HOLD       BUTTON_1
#define BUTTON_DFU_HOLD_MS    3000

/* With no RESET button there is also no way to reset out of UF2 mode once in
 * it, and the host just re-mounts the drive if we stay enumerated. Ejecting the
 * drive therefore leaves DFU and boots the application. */
#define UF2_EXIT_ON_EJECT

//--------------------------------------------------------------------+
// BLE OTA
//--------------------------------------------------------------------+
#define BLEDIS_MANUFACTURER   "Seeed Studio"
#define BLEDIS_MODEL          "MeshTracker-X1"

//--------------------------------------------------------------------+
// USB
//--------------------------------------------------------------------+
#define USB_DESC_VID            0x2886
#define USB_DESC_UF2_PID        0x0057
#define USB_DESC_CDC_ONLY_PID   0x0057

//--------------------------------------------------------------------+
// UF2
//--------------------------------------------------------------------+
#define UF2_PRODUCT_NAME        "Seeed MeshTracker-X1 for Meshtastic"
#define UF2_VOLUME_LABEL        "X1"
#define UF2_BOARD_ID            "nRF52840-MeshTracker-X1"
#define UF2_INDEX_URL           "https://wiki.seeedstudio.com/meshtracker_x1_node"


#endif /* _MESH_TRACKER_X1_H */
