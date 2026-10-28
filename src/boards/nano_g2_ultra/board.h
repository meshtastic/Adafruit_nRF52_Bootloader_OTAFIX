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

#ifndef _NANO_G2_ULTRA_H
#define _NANO_G2_ULTRA_H

#define _PINNUM(port, pin)    ((port)*32 + (pin))

/*------------------------------------------------------------------*/
/* LED
 *------------------------------------------------------------------*/
// No LED pins found in meshtastic/firmware's variants/nrf52840/nano-g2-ultra
// (variant.h / variant.cpp) - this board appears to have no onboard LED,
// relying on its SH1107 OLED for status instead. LEDS_NUMBER=0 is a
// supported configuration (see src/boards/boards.c's `#if LEDS_NUMBER > 0`
// guards) - UNCONFIRMED on real hardware, flag in review if wrong.
#define LEDS_NUMBER           0

/*------------------------------------------------------------------*/
/* BUTTON
 *------------------------------------------------------------------*/
// firmware's variant.h defines exactly one physical button, PIN_BUTTON1 =
// _PINNUM(1, 6). src/boards/boards.h hard-requires BUTTONS_NUMBER >= 2, so
// - mirroring wiscore_rak4631_board's own pattern of reusing one pin for
//   both slots when the board has only one (or no) real button - both
//   BUTTON_1 and BUTTON_2 point at the same physical button here. That
//   means holding it through a reset will trigger this bootloader's
//   forced-OTA-DFU combo (BUTTON_DFU && BUTTON_FRESET, see src/main.c).
//   UNCONFIRMED on real hardware.
#define BUTTONS_NUMBER        2
#define BUTTON_1              _PINNUM(1, 6)
#define BUTTON_2              _PINNUM(1, 6)
#define BUTTON_PULL           NRF_GPIO_PIN_PULLUP

//--------------------------------------------------------------------+
// BLE OTA
//--------------------------------------------------------------------+
#define BLEDIS_MANUFACTURER   "BQ Consulting"
#define BLEDIS_MODEL          "Nano G2 Ultra"

//--------------------------------------------------------------------+
// USB
//--------------------------------------------------------------------+
// VID 0x239A is Adafruit's, reused here rather than a BQ Consulting VID -
// matches this board's own PlatformIO/Arduino-core board manifest
// (datapartyjs/MeshTNC's boards/nano-g2-ultra.json), which lists hwids
// under 0x239A too.
//
// UF2_PID is NOT independently confirmed - no dedicated allocation for
// this board was found. 0x00E2 was picked because it doesn't collide with
// any other USB_DESC_UF2_PID already used under 0x239A in this repo
// (0x0029, 0x0071, 0x00B3, 0x00DA at time of writing). If BQ Consulting
// has actually registered a PID for this board, use that instead.
#define USB_DESC_VID           0x239A
#define USB_DESC_UF2_PID       0x00E2
#define USB_DESC_CDC_ONLY_PID  0x00E3

//------------- UF2 -------------//
#define UF2_PRODUCT_NAME      "Nano G2 Ultra"
#define UF2_VOLUME_LABEL      "NANOG2ULTRA"
#define UF2_BOARD_ID          "BQ-NanoG2Ultra"
#define UF2_INDEX_URL         "https://wiki.uniteng.com/en/meshtastic/nano-g2-ultra"

#endif // _NANO_G2_ULTRA_H
