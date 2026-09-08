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

#ifndef _T1000_E_H
#define _T1000_E_H

#define _PINNUM(port, pin)    ((port)*32 + (pin))

/*------------------------------------------------------------------*/
/* LED
 *------------------------------------------------------------------*/
#define LEDS_NUMBER           1
#define LED_PRIMARY_PIN       _PINNUM(0, 24)  // Green
#define LED_STATE_ON          0

/*------------------------------------------------------------------*/
/* BUTTON
 *------------------------------------------------------------------*/
/* P0.06 is active high with an internal pull-down. Firmware agrees:
 * variants/nrf52840/tracker-t1000-e/variant.h sets BUTTON_ACTIVE_LOW false and
 * BUTTON_SENSE_TYPE 0x5. The previous NRF_GPIO_PIN_PULLUP made button_pressed()
 * compare against an active_state of 0 on a pin that can only ever read 1, so
 * the button was never seen here at all.
 *
 * P0.18 is RESET and is not exposed as a second button, so double-reset entry
 * is unavailable and the primary button is the only way in. A momentary press
 * belongs to the application, so this board uses the hold-to-enter scheme (see
 * BUTTON_DFU_HOLD in src/main.c), which also compiles out the BUTTON_FRESET
 * reads - P0.18 sits high and would otherwise read as permanently pressed.
 * BUTTON_2 is aliased only to satisfy the BUTTONS_NUMBER >= 2 check. */
#define BUTTONS_NUMBER        2
#define BUTTON_1              _PINNUM(0, 6)  // Primary Button
#define BUTTON_2              BUTTON_1       // no usable second button, P0.18 is RESET
#define BUTTON_PULL           NRF_GPIO_PIN_PULLDOWN

// Hold the primary button through boot for this long to force UF2 DFU.
#define BUTTON_DFU_HOLD       BUTTON_1
#define BUTTON_DFU_HOLD_MS    3000

/* With no RESET button there is also no way to reset out of UF2 mode once in
 * it, and the host just re-mounts the drive if we stay enumerated. Ejecting the
 * drive therefore leaves DFU and boots the application. */
#define UF2_EXIT_ON_EJECT

/* P0.18 is RESET and is not exposed as a button, so there is no way to reset
 * out of UF2 mode once in it, and the host just re-mounts the drive if we stay
 * enumerated. Ejecting the drive therefore leaves DFU and boots the
 * application. */
#define UF2_EXIT_ON_EJECT

//--------------------------------------------------------------------+
// BLE OTA
//--------------------------------------------------------------------+
#define BLEDIS_MANUFACTURER   "Seeed Studio"
#define BLEDIS_MODEL          "T1000-E"

//--------------------------------------------------------------------+
// USB
//--------------------------------------------------------------------+
#define USB_DESC_VID            0x2886
#define USB_DESC_UF2_PID        0x0057
#define USB_DESC_CDC_ONLY_PID   0x0057

//--------------------------------------------------------------------+
// UF2
//--------------------------------------------------------------------+
#define UF2_PRODUCT_NAME        "Seeed T1000-E for Meshtastic"
#define UF2_VOLUME_LABEL        "T1000-E"
#define UF2_BOARD_ID            "nRF52840-T1000-E-v1"
#define UF2_INDEX_URL           "https://www.seeedstudio.com/SenseCAP-Card-Tracker-T1000-E-for-Meshtastic-p-5913.html"


#endif /* _T1000_E_H */
