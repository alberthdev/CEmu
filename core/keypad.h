#ifndef KEYPAD_H
#define KEYPAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "port.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/* Standard KEYPAD state */
typedef struct keypad_state {
    union {
        struct {
            uint16_t mode : 2, row_wait : 14, scan_wait;
        };
        uint32_t control;
    };
    union {
        struct {
            uint8_t rows, cols;
        };
        uint32_t size;
    };
    uint8_t  current_row;
    uint8_t  status;
    uint8_t  enable;
    uint16_t data[16];
    uint16_t keyMap[16];
    uint16_t delay[16];
    uint32_t gpio_status;
    uint32_t gpio_enable;
} keypad_state_t;

/* Global KEYPAD state */
extern keypad_state_t keypad;

/* Available Functions */
eZ80portrange_t init_keypad(void);
void keypad_intrpt_check(void);
void keypad_reset(void);
void keypad_key_event(unsigned int row, unsigned int col, bool press);

/* Save/Restore */
bool keypad_restore(FILE *image);
bool keypad_save(FILE *image);

#ifdef __cplusplus
}
#endif

#endif
