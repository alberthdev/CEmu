#ifndef CONTROLPORT_H
#define CONTROLPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "apb.h"

enum {
    BATTERY_DISCHARGED,
    BATTERY_0,
    BATTERY_1,
    BATTERY_2,
    BATTERY_3,
    BATTERY_4
};

typedef struct control_state {
    uint8_t ports[0x80];
    uint8_t cpuSpeed;
    bool noPlugAInserted;
    bool USBConnected;

    uint8_t setBatteryStatus;
    uint8_t readBatteryStatus;
    bool batteryCharging;
} control_state_t;

/* Global CONTROL state */
extern control_state_t control;

/* Available Functions */
void free_control(void *_state);
eZ80portrange_t init_control(void);

#ifdef __cplusplus
}
#endif

#endif
