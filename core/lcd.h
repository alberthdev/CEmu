#ifndef LCD_LCD_H
#define LCD_LCD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "port.h"
#include "spi.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define LCD_RGB_SIZE   3
#define LCD_WIDTH      320
#define LCD_HEIGHT     240
#define LCD_SIZE       (LCD_WIDTH * LCD_HEIGHT)
#define LCD_BYTE_SIZE  (LCD_SIZE * 2)
#define LCD_RAM_ADDR   0xD40000
#define LCD_RAM_OFFSET 0x040000
#define LCD_FRAME_SIZE (LCD_SIZE * LCD_RGB_SIZE)

enum lcd_comp {
    LCD_SYNC,
    LCD_BACK,
    LCD_ACTIVE,
    LCD_FRONT
};

/* Standard LCD state */
typedef struct lcd_state {
    uint32_t timing[4];

    uint32_t control;             /* Control register */
    uint32_t imsc;                /* Interrupt mask set/clear register */
    uint32_t ris;

    uint32_t upbase;               /* Upper panel frame base address register */
    uint32_t lpbase;               /* Lower panel frame base address register */
    uint32_t upcurr;               /* Upper panel current frame address register */
    uint32_t lpcurr;               /* Lower panel current frame address register */

    /* 256x16-bit color palette registers */
    /* 256 palette entries organized as 128 locations of two entries per word */
    uint16_t palette[0x100];

    /* Cursor image RAM registers */
    /* 256-word wide values defining images overlaid by the hw cursor mechanism */
    uint32_t crsrImage[0x100];
    uint32_t crsrControl;          /* Cursor control register */
    uint32_t crsrConfig;           /* Cursor configuration register */
    uint32_t crsrPalette0;         /* Cursor palette registers */
    uint32_t crsrPalette1;
    uint32_t crsrXY;               /* Cursor XY position register */
    uint32_t crsrClip;             /* Cursor clip position register */
    uint32_t crsrImsc;             /* Cursor interrupt mask set/clear register */
    uint32_t crsrIcr;              /* Cursor interrupt clear register */
    uint32_t crsrRis;              /* Cursor raw interrupt status register - const */

    /* Internal registers */
    //enum lcd_comp cur_stage;
    uint32_t PPL, HSW, HFP, HBP, LPP, VSW, VFP, VBP, PCD, ACB, CPL, LED;
    bool CLKSEL, IVS, IHS, IPC, IOE, LEE, mask;
    uint32_t width;
    uint32_t height;
    uint32_t size;
    uint32_t *ofs;                  /* Pointer to start of data to start extracting from */
    uint32_t *ofs_end;              /* End pointer that is allowed access */
    uint32_t frame[LCD_SIZE];       /* Location on which to draw the data */
} lcd_state_t;

/* Global LCD state */
extern lcd_state_t lcd;

/* Available Functions */
void lcd_reset(void);
eZ80portrange_t init_lcd(void);

void lcd_drawframe(uint32_t *out, lcd_state_t*);
void lcd_setptrs(lcd_state_t*);

void lcd_enable(void);
void lcd_disable(void);

/* Save/Restore */
bool lcd_restore(FILE *image);
bool lcd_save(FILE *image);

#ifdef __cplusplus
}
#endif

#endif
