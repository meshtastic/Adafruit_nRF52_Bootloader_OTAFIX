/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Ha Thach (tinyusb.org) for Adafruit Industries
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

#include "boards.h"

#ifdef BOARD_HAS_DISPLAY

// Defaults for 1bpp mono panels. The stock geometry below is sized for a
// 240x135 TFT and does not fit a typical 128x64 OLED, so give mono boards a
// four-line layout (title / mode / version / banner) out of the box. A board
// only has to declare its bus and title; anything here can still be
// overridden per board.
#ifdef DISPLAY_MONO
#if DISPLAY_HEIGHT < 96
// Three 32x32 icons need 96x32; that plus four text lines does not fit.
#define DISPLAY_NO_DRAG_ICONS
#define NOLABELS
#endif

#ifndef FONT_SIZE_LARGE
#define FONT_SIZE_LARGE      2
#endif
#ifndef BANNER_TEXT
#define BANNER_TEXT          "meshtastic.org"
#endif

// The colour bars are dark in mono, so bar 1 simply spans the panel and the
// other two collapse; this also keeps drawBar inside the frame buffer.
#ifndef SCREEN_BAR1_Y
#define SCREEN_BAR1_Y        0
#endif
#ifndef SCREEN_BAR1_H
#define SCREEN_BAR1_H        DISPLAY_HEIGHT
#endif
#ifndef SCREEN_BAR2_H
#define SCREEN_BAR2_H        0
#endif
#ifndef SCREEN_BAR3_H
#define SCREEN_BAR3_H        0
#endif

#ifndef DISPLAY_TITLE_Y
#define DISPLAY_TITLE_Y      0
#endif
#ifndef DRAG_TEXT_Y
#define DRAG_TEXT_Y          22
#endif
#ifndef BLE_OTA_Y
#define BLE_OTA_Y            22
#endif
#ifndef UF2_VERSION_BASE_Y
#define UF2_VERSION_BASE_Y   44
#endif
#ifndef BANNER_TEXT_Y
#define BANNER_TEXT_Y        55
#endif
#endif // DISPLAY_MONO

#ifndef BANNER_TEXT
#define BANNER_TEXT "meshtastic.org - OTAFIX by oltaco"
#endif //BANNER_TEXT

#include <string.h>
#include <stdlib.h>

// Size independent kerning
#define CHAR_ADV(n)  (5 * (n) + 1)   // cursor step between chars
#define CHAR_INK(n)  (6 * (n))       // columns printch actually writes
#define TEXT_WIDTH(n, len)  ((len) ? CHAR_ADV(n) * ((len) - 1) + CHAR_INK(n) : 0)

#define COL0(r, g, b) ((((r) >> 3) << 11) | (((g) >> 2) << 5) | ((b) >> 3))
#define COL(c) COL0((c >> 16) & 0xff, (c >> 8) & 0xff, c & 0xff)

enum {
  COLOR_BLACK = 0,
  COLOR_WHITE = 1,
  COLOR_RED = 2,
  COLOR_PINK = 3,
  COLOR_ORANGE = 4,
  COLOR_YELLOW = 5,
  COLOR_CYAN = 6,
  COLOR_GREEN = 7,
  COLOR_BLUE = 8,
  COLOR_AQUA = 9,
  COLOR_PURPLE = 10,
};

// 16-bit 565 color from 24-bit 888 format
const uint16_t palette[] = {
    COL(0x000000), // 0
    COL(0xffffff), // 1
    COL(0xff2121), // 2
    COL(0xff93c4), // 3
    COL(0xff8135), // 4
    COL(0xfff609), // 5
    COL(0x249ca3), // 6
    COL(0x78dc52), // 7
    COL(0x003fad), // 8
    COL(0x87f2ff), // 9
    COL(0x8e2ec4), // 10

    COL(0xa4839f), // 11
    COL(0x5c406c), // 12
    COL(0xe5cdc4), // 13
    COL(0x91463d), // 14
    COL(0x000000), // 15
};

// inital display-size agnostic hack
#ifndef SCREEN_BAR1_Y
#define SCREEN_BAR1_Y       0
#endif
#ifndef SCREEN_BAR1_H
#define SCREEN_BAR1_H       52
#endif

#ifndef SCREEN_BAR2_Y
#define SCREEN_BAR2_Y       52
#endif
#ifndef SCREEN_BAR2_H
#define SCREEN_BAR2_H       55
#endif

#ifndef SCREEN_BAR3_Y
#define SCREEN_BAR3_Y       107
#endif
#ifndef SCREEN_BAR3_H
#define SCREEN_BAR3_H       14
#endif
#ifndef fileLogo_X
#define fileLogo_X           0
#endif
#ifndef arrowLogo_X
#define arrowLogo_X          65
#endif
#ifndef pendriveLogo_X
#define pendriveLogo_X       129
#endif
#ifndef DISPLAY_TITLE_Y
#define DISPLAY_TITLE_Y      5
#endif
#ifndef UF2_VERSION_BASE_Y
#define UF2_VERSION_BASE_Y   40
#endif
#ifndef BANNER_TEXT_X
#define BANNER_TEXT_X        23
#endif
#ifndef BANNER_TEXT_Y
#define BANNER_TEXT_Y        110
#endif
#ifndef BLE_OTA_Y
#define BLE_OTA_Y            65
#endif
// Mode line on the drag screen, for panels too short for the 32px icon row.
#ifndef DRAG_TEXT_Y
#define DRAG_TEXT_Y          BLE_OTA_Y
#endif
#ifndef DRAG_TEXT
#define DRAG_TEXT            "USB UF2"
#endif
#ifndef FONT_SIZE_LARGE
#define FONT_SIZE_LARGE      4
#endif


// TODO only buffer partial screen to save SRAM
// ESP32s2 can only statically allocated DRAM up to 160KB.
// the remaining 160KB can only be allocated at runtime as heap.
#ifdef DISPLAY_MONO
_Static_assert(DISPLAY_HEIGHT % 8 == 0,
               "mono panels are addressed in 8-row pages; DISPLAY_HEIGHT must be a multiple of 8");
#endif

static uint8_t frame_buf[DISPLAY_WIDTH * DISPLAY_HEIGHT];
//static uint8_t* frame_buf;

extern const uint8_t font8[];
extern const uint8_t fileLogo[];
extern const uint8_t pendriveLogo[];
extern const uint8_t arrowLogo[];

//--------------------------------------------------------------------+
//
//--------------------------------------------------------------------+

// print character
static void printch(int x, int y, int color, const uint8_t* fnt, int size) {
  for (int i = 0; i < 6 * size; ++i) {
    uint8_t* p = frame_buf + (x + i) * DISPLAY_HEIGHT + y;
    uint8_t b = fnt[i / size];
    for (int j = 0; j < 8; ++j) {
      if (b & (1u << j)) {
        for (int k = 0; k < size; ++k) p[k] = color;
      }
      p += size;
    }
  }
}

#ifndef DISPLAY_NO_DRAG_ICONS
// print icon
static void printicon(int x, int y, int color, const uint8_t* icon) {
  int w = *icon++;
  int h = *icon++;
  int sz = *icon++;

  // The decode loop below writes w*h bytes straight into frame_buf with no
  // clipping of its own, so anything that would land outside is dropped.
  if (x < 0 || y < 0 || x + w > DISPLAY_WIDTH || y + h > DISPLAY_HEIGHT) {
    return;
  }

  uint8_t mask = 0x80;
  int runlen = 0;
  int runbit = 0;
  uint8_t lastb = 0x00;

  for (int i = 0; i < w; ++i) {
    uint8_t* p = frame_buf + (x + i) * DISPLAY_HEIGHT + y;
    for (int j = 0; j < h; ++j) {
      int c = 0;
      if (mask != 0x80) {
        if (lastb & mask) {
          c = 1;
        }
        mask <<= 1;
      } else if (runlen) {
        if (runbit) {
          c = 1;
        }
        runlen--;
      } else {
        if (sz-- <= 0) {
          //TU_LOG1("Screen Panic code = 10");
        }
        lastb = *icon++;
        if (lastb & 0x80) {
          runlen = lastb & 63;
          runbit = lastb & 0x40;
        } else {
          mask = 0x01;
        }
        --j;
        continue; // restart
      }
      if (c) {
        *p = color;
      }
      p++;
    }
  }
}
#endif // DISPLAY_NO_DRAG_ICONS

static void print(int x, int y, int color, const char* text, int size) {
  // Glyphs are written straight into frame_buf with no bounds check of
  // their own, so reject anything that would land outside it.
  if (y < 0 || y + 8 * size > DISPLAY_HEIGHT) {
    return;
  }
  while (*text) {
    char c = *text++;
    if (c < ' ' || c >= 0x7f) c = '?';
    c -= ' ';
    if (x < 0 || x + CHAR_INK(size) > DISPLAY_WIDTH) {
      // This char won't fit.
      return;
    }
    printch(x, y, color, &font8[c * 6], size);
    x += CHAR_ADV(size);
  }
}

static inline void print_centered(int y, int color, const char* text, int size) {
  int len = (int) strlen(text);
  int text_x = (DISPLAY_WIDTH - TEXT_WIDTH(size, len)) / 2;
  print(text_x >= 0 ? text_x : 0, y, color, text, size);
}

//--------------------------------------------------------------------+
//
//--------------------------------------------------------------------+

#ifdef DISPLAY_MONO

// One bit per pixel, so the 16-entry palette collapses to "lights a pixel or
// does not". The layout above only ever draws foreground in white (text,
// icons) and purple (the version string); the three background bars use
// colours that stay dark. That is what lets the colour layout survive the
// conversion without being redrawn for mono.
#define MONO_LIT_MASK   ((1u << COLOR_WHITE) | (1u << COLOR_PURPLE))

static void draw_screen(uint8_t const* fb) {
  // SSD1306/SH1106 GDDRAM is paged: one byte spans 8 rows of a single
  // column, LSB topmost. frame_buf is already column-major, so each output
  // byte gathers 8 contiguous bytes of it.
  for (int page = 0; page < DISPLAY_HEIGHT / 8; ++page) {
    uint8_t line[DISPLAY_WIDTH];

    for (int x = 0; x < DISPLAY_WIDTH; ++x) {
      uint8_t const* col = fb + x * DISPLAY_HEIGHT + page * 8;
      uint8_t bits = 0;
      for (int bit = 0; bit < 8; ++bit) {
        if (MONO_LIT_MASK & (1u << (col[bit] & 0xf))) {
          bits |= (uint8_t) (1u << bit);
        }
      }
      line[x] = bits;
    }

    board_display_draw_page((uint8_t) page, line, sizeof(line));
  }
}

#else

static void draw_screen(uint8_t const* fb) {
  uint8_t const* p = fb;
  for (int y = 0; y < DISPLAY_WIDTH; ++y) {
    uint8_t cc[DISPLAY_HEIGHT * 2];
    uint32_t dst = 0;
    for (int x = 0; x < DISPLAY_HEIGHT; ++x) {
      uint16_t color = palette[*p++ & 0xf];
      cc[dst++] = color >> 8;
      cc[dst++] = color & 0xff;
    }

    board_display_draw_line(y, cc, sizeof(cc));
  }
}

#endif

// draw color bar
static void drawBar(int y, int h, int color) {
  // Clamp to the panel: the default bar geometry is sized for a 240x135
  // screen and would run off the end of a column on a shorter one.
  if (y < 0) {
    h += y;
    y = 0;
  }
  if (y >= DISPLAY_HEIGHT || h <= 0) {
    return;
  }
  if (y + h > DISPLAY_HEIGHT) {
    h = DISPLAY_HEIGHT - y;
  }

  for (int x = 0; x < DISPLAY_WIDTH; ++x) {
    memset(frame_buf + x * DISPLAY_HEIGHT + y, color, h);
  }
}

// draw drag & drop screen
void screen_draw_drag(void) {
  drawBar(SCREEN_BAR1_Y, SCREEN_BAR1_H, COLOR_GREEN);
  drawBar(SCREEN_BAR2_Y, SCREEN_BAR2_H, COLOR_BLUE);
  drawBar(SCREEN_BAR3_Y, SCREEN_BAR3_H, COLOR_ORANGE);

  // Print title, version, banner.
  print_centered(DISPLAY_TITLE_Y, COLOR_WHITE, DISPLAY_TITLE, FONT_SIZE_LARGE);
  print_centered(UF2_VERSION_BASE_Y, COLOR_PURPLE, UF2_VERSION_BASE, 1);
  print_centered(BANNER_TEXT_Y, COLOR_WHITE, BANNER_TEXT, 1);

#ifdef DISPLAY_NO_DRAG_ICONS
  // The icon row is three 32x32 glyphs, 96px wide and 32px tall before any
  // spacing. A 128x64 panel cannot carry that and the four text lines, so
  // name the mode instead.
  print_centered(DRAG_TEXT_Y, COLOR_WHITE, DRAG_TEXT, FONT_SIZE_LARGE);
#else
#ifndef DRAG
#define DRAG 70
#endif
#ifndef DRAGX
#define DRAGX 47
#endif
  printicon(DRAGX + fileLogo_X, DRAG + 5, COLOR_WHITE, fileLogo);
  printicon(DRAGX + arrowLogo_X, DRAG, COLOR_WHITE, arrowLogo);
  printicon(DRAGX + pendriveLogo_X, DRAG, COLOR_WHITE, pendriveLogo);
  #ifndef NOLABELS
  print(22, DRAG - 12, COLOR_WHITE, "firmware.uf2", 1);
  print(160, DRAG - 12, COLOR_WHITE, UF2_VOLUME_LABEL, 1);
  #endif // NOLABELS
#endif // DISPLAY_NO_DRAG_ICONS

  draw_screen(frame_buf);
}

void screen_draw_ble(void) {
  drawBar(SCREEN_BAR1_Y, SCREEN_BAR1_H, COLOR_GREEN);
  drawBar(SCREEN_BAR2_Y, SCREEN_BAR2_H, COLOR_BLUE);
  drawBar(SCREEN_BAR3_Y, SCREEN_BAR3_H, COLOR_ORANGE);

  // Print title, version, OTA, banner
  print_centered(DISPLAY_TITLE_Y, COLOR_WHITE, DISPLAY_TITLE, FONT_SIZE_LARGE);
  print_centered(UF2_VERSION_BASE_Y, COLOR_PURPLE, UF2_VERSION_BASE, 1);
  print_centered(BLE_OTA_Y, COLOR_WHITE, "BLE OTA", FONT_SIZE_LARGE);
  print_centered(BANNER_TEXT_Y, COLOR_WHITE, BANNER_TEXT, 1);

  draw_screen(frame_buf);
}

#endif
