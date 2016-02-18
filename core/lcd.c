/* Copyright (C) 2015  Fabian Vogt
 * Modified for the CE calculator by CEmu developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
*/

#include <string.h>

#include "lcd.h"
#include "schedule.h"
#include "interrupt.h"
#include "cpu.h"
#include "emu.h"

/* Global LCD state */
lcd_state_t lcd;

static const uint32_t vram_size = 320 * 240 * 2;
static const uint32_t lcd_dma_size = 0x80000;

void (*lcd_event_gui_callback)(void) = NULL;

inline uint_fast32_t lcd_nextword(uint32_t **ofs) {
    uint_fast32_t _ofs = (uint_fast32_t) ((*ofs)++);
    if (_ofs < ram_size) {
        return *(uint32_t *) (mem.ram.block + _ofs);
    }
    if (_ofs == lcd_dma_size) {
        *ofs = 0;
    }
    return 0;
}

// #define c6_to_c8(c) ((c * 0xFF + 0x1F) / 0x3F)
#define c6_to_c8(c) ((c << 2) | (c >> 4))

inline void lcd_bgr16out(uint_fast32_t bgr16, bool rgb, uint32_t **out) {
    uint_fast32_t r, g, b;

    r = (rgb ? bgr16 >> 10 : bgr16 << 1) & 0x3E;
    r |= r >> 5;
    r = c6_to_c8(r);

    g = bgr16 >> 5 & 0x3F;
    g = c6_to_c8(g);

    b = (rgb ? bgr16 << 1 : bgr16 >> 10) & 0x3E;
    b |= b >> 5;
    b = c6_to_c8(b);

    *(*out)++ = r | (g << 8) | (b << 16) | (0xFFu << 24);
}

/* Draw the current screen into a 320*240*4-byte RGBA8888 buffer. Alpha is always 255. */
void lcd_drawframe(uint32_t *out) {
    uint_fast8_t mode = lcd.control >> 1 & 7;
    bool rgb = lcd.control & (1 << 8);
    bool bebo = lcd.control & (1 << 9);
    uint_fast32_t words = 320 * 240;
    uint_fast32_t word, color;
    uint32_t *ofs = (uint32_t *) ((uint32_t) lcd.upcurr & (lcd_dma_size - 8));

    if(!mem.ram.block) {
        memset(out, 0, 320 * 240 * 4);
        return;
    }

    if (mode < 4) {
        uint_fast8_t bpp = 1 << mode;
        uint_fast32_t mask = (1 << bpp) - 1;
        uint_fast8_t bi = bebo ? 0 : 24;
        bool bepo = lcd.control & (1 << 10);
        if (!bepo) {
            bi ^= (8 - bpp);
        }
        do {
            uint_fast8_t bitpos = 32;
            word = lcd_nextword(&ofs);
            do {
                color = lcd.palette[word >> ((bitpos -= bpp) ^ bi) & mask];
                lcd_bgr16out(color + (color & 0xFFE0) + (color >> 10 & 0x20), rgb, &out);
                words--;
            } while (bitpos != 0);
        } while (words != 0);

    } else if (mode == 4) {
        do {
            uint_fast8_t i = 2;
            word = lcd_nextword(&ofs);
            if (bebo) {
                word = word << 16 | word >> 16;
            }
            do {
                color = word;
                lcd_bgr16out(color + (color & 0xFFE0) + (color >> 10 & 0x20), rgb, &out);
                word >>= 16;
                words--;
            } while (--i != 0);
        } while (words != 0);

    } else if (mode == 5) {
        do {
            word = lcd_nextword(&ofs);
            lcd_bgr16out((word >> 8 & 0xF800) | (word >> 5 & 0x7E0) | (word >> 3 & 0x1F), rgb, &out);
            words--;
        } while (words != 0);

    } else if (mode == 6) {
        do {
            uint_fast8_t i = 2;
            word = lcd_nextword(&ofs);
            if (bebo) {
                word = word << 16 | word >> 16;
            }
            do {
                lcd_bgr16out(word, rgb, &out);
                word >>= 16;
                words--;
            } while (--i != 0);
        } while (words != 0);

    } else {
        do {
            uint_fast8_t i = 2;
            word = lcd_nextword(&ofs);
            if (bebo) {
                word = word << 16 | word >> 16;
            }
            do {
                lcd_bgr16out((word << 4 & 0xF000) | (word << 3 & 0x780) | (word << 1 & 0x1E), rgb, &out);
                word >>= 16;
                words--;
            } while (--i != 0);
        } while (words != 0);
    }
}

static void lcd_event(int index) {
    int pcd = 1;
    int htime, vtime;

    if (!(lcd.timing[2] & (1 << 26))) {
        pcd = (lcd.timing[2] >> 27 << 5) + (lcd.timing[2] & 0x1F) + 2;
    }

    htime =   (lcd.timing[0] >> 24 & 0x0FF) + 1  /* Back porch    */
            + (lcd.timing[0] >> 16 & 0x0FF) + 1  /* Front porch   */
            + (lcd.timing[0] >>  8 & 0x0FF) + 1  /* Sync pulse    */
            + (lcd.timing[2] >> 16 & 0x3FF) + 1; /* Active        */
    vtime =   (lcd.timing[1] >> 24 & 0x0FF)      /* Back porch    */
            + (lcd.timing[1] >> 16 & 0x0FF)      /* Front porch   */
            + (lcd.timing[1] >> 10 & 0x03F) + 1  /* Sync pulse    */
            + (lcd.timing[1]       & 0x3FF) + 1; /* Active        */
    event_repeat(index, pcd * htime * vtime);

    /* For now, assuming vcomp occurs at same time UPBASE is loaded */
    lcd.upcurr = lcd.upbase;
    lcd.ris |= 0xC;
    intrpt_set(INT_LCD, lcd.ris & lcd.mis);

    if (lcd_event_gui_callback) {
        lcd_event_gui_callback();
    }
}

void lcd_reset(void) {
    /* Palette is unchanged on a reset */
    memset(&lcd, 0, (char *)&lcd.palette - (char *)&lcd);
    sched.items[SCHED_LCD].clock = CLOCK_12M;
    sched.items[SCHED_LCD].second = -1;
    sched.items[SCHED_LCD].proc = lcd_event;
    gui_console_printf("[CEmu] LCD reset.\n");
}

uint8_t lcd_read(const uint16_t pio) {
    uint16_t index = pio & 0xFFF;
    uint8_t bit_offset = (index & 3) << 3;

    if (index < 0x200) {
        if(index < 0x010) { return read8(lcd.timing[index >> 2], bit_offset); }
        if(index < 0x014 && index >= 0x010) { return read8(lcd.upbase, bit_offset); }
        if(index < 0x018 && index >= 0x014) { return read8(lcd.lpbase, bit_offset); }
        if(index < 0x01C && index >= 0x018) { return read8(lcd.control, bit_offset); }
        if(index < 0x020 && index >= 0x01C) { return read8(lcd.imsc, bit_offset); }
        if(index < 0x024 && index >= 0x020) { return read8(lcd.ris, bit_offset); }
        if(index < 0x028 && index >= 0x024) { return read8(lcd.mis & lcd.ris, bit_offset); }
    } else if (index < 0x400) {
        return *((uint8_t *)lcd.palette + index - 0x200);
    } else if (index < 0xC30) {
        if(index < 0xC00 && index >= 0x800) { return read8(lcd.crsrImage[((pio-0x800) & 0x3FF) >> 2], bit_offset); }
        if(index == 0xC00) { return read8(lcd.crsrControl, bit_offset); }
        if(index == 0xC04) { return read8(lcd.crsrConfig, bit_offset); }
        if(index < 0xC0C && index >= 0xC08) { return read8(lcd.crsrPalette0, bit_offset); }
        if(index < 0xC10 && index >= 0xC0C) { return read8(lcd.crsrPalette1, bit_offset); }
        if(index < 0xC14 && index >= 0xC10) { return read8(lcd.crsrXY, bit_offset); }
        if(index < 0xC16 && index >= 0xC14) { return read8(lcd.crsrClip, bit_offset); }
        if(index == 0xC20) { return read8(lcd.crsrImsc, bit_offset); }
        if(index == 0xC28) { return read8(lcd.crsrRis, bit_offset); }
        if(index == 0xC2C) { return read8(lcd.crsrRis & lcd.crsrImsc, bit_offset); }
    } else if (index >= 0xFE0) {
        static const uint8_t id[1][8] = {
            { 0x11, 0x11, 0x14, 0x00, 0x0D, 0xF0, 0x05, 0xB1 }
        };
        return read8(id[0][(index - 0xFE0) >> 2], bit_offset);
    }

    /* Return 0 if bad read */
    return 0;
}

void lcd_write(const uint16_t pio, const uint8_t value) {
    uint32_t index = pio & 0xFFC;

    uint8_t byte_offset = pio & 3;
    uint8_t bit_offset = byte_offset << 3;

    if (index < 0x200) {
        if (index < 0x010) {
            write8(lcd.timing[index >> 2], bit_offset, value);
        } else if (index < 0x014 && index >= 0x010) {
            write8(lcd.upbase, bit_offset, value);
            if (lcd.upbase & 7) {
                gui_console_printf("[CEmu] Warning: LCD upper panel base not 8-byte aligned\n");
            }
            lcd.upbase &= ~7U;
        } else if (index < 0x018 && index >= 0x014) {
            write8(lcd.lpbase, bit_offset, value);
            if (lcd.lpbase & 7) {
                gui_console_printf("[CEmu] Warning: LCD lower panel base not 8-byte aligned\n");
            }
            lcd.lpbase &= ~7U;
        } else if (index == 0x018) {
            if(byte_offset == 0) {
                if (value & 1) { event_set(SCHED_LCD, 0); }
                else { event_clear(SCHED_LCD); }
            }
            write8(lcd.control, bit_offset, value);
        } else if (index == 0x01C) {
            write8(lcd.imsc, bit_offset, value);
            lcd.imsc &= 0x1E;
            intrpt_set(INT_LCD, lcd.ris & lcd.imsc);
        } else if (index == 0x028) {
            lcd.ris &= ~(value << bit_offset);
            intrpt_set(INT_LCD, lcd.ris & lcd.imsc);
        }
    } else if (index < 0x400) {
        write8(lcd.palette[pio >> 1 & 0xFF], (pio & 1) << 3, value);
    } else if (index < 0xC30) {
        if(index < 0xC00 && index >= 0x800) {
            write8(lcd.crsrImage[((pio-0x800) & 0x3FF) >> 2], bit_offset, value);
        }
        if(index == 0xC00) {
            write8(lcd.crsrControl, bit_offset, value);
        }
        if(index == 0xC04) {
            write8(lcd.crsrConfig, bit_offset, value);
            lcd.crsrConfig &= 0xF;
        }
        if(index < 0xC0C && index >= 0xC08) {
            write8(lcd.crsrPalette0, bit_offset, value);
        }
        if(index < 0xC10 && index >= 0xC0C) {
            write8(lcd.crsrPalette1, bit_offset, value);
        }
        if(index < 0xC14 && index >= 0xC10) {
            write8(lcd.crsrXY, bit_offset, value);
        }
        if(index < 0xC16 && index >= 0xC14) {
            write8(lcd.crsrClip, bit_offset, value);
        }
        if(index == 0xC20) {
            write8(lcd.crsrImsc, bit_offset, value);
        }
        if(index == 0xC24) {
            lcd.crsrRis &= ~(value << bit_offset);
        }
    }
}

static const eZ80portrange_t device = {
    .read_in    = lcd_read,
    .write_out  = lcd_write
};

eZ80portrange_t init_lcd(void) {
    gui_console_printf("[CEmu] Initialized LCD...\n");
    return device;
}

bool lcd_save(emu_image *s) {
    s->lcd = lcd;
    return true;
}

bool lcd_restore(const emu_image *s) {
    lcd = s->lcd;
    return true;
}
