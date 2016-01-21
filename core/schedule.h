#ifndef SCHEDULE_H
#define SCHEDULE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

enum clock_id { CLOCK_CPU, CLOCK_APB, CLOCK_27M, CLOCK_12M, CLOCK_32K };

enum sched_item_index {
    SCHED_THROTTLE,
    SCHED_KEYPAD,
    SCHED_LCD,
    SCHED_RTC,
    SCHED_OSTIMER,
    SCHED_TIMER1,
    SCHED_TIMER2,
    SCHED_TIMER3,
    SCHED_WATCHDOG,
    SCHED_NUM_ITEMS
};

struct sched_item {
    enum clock_id clock;
    int second; /* -1 = disabled */
    uint32_t tick;
    uint32_t cputick;
    void (*proc)(int index);
};

typedef struct sched_state {
    struct sched_item items[SCHED_NUM_ITEMS];
    uint32_t clockRates[6];
    uint32_t nextCPUtick;
    int nextIndex; /* -1 if no more events this second */
} sched_state_t;

extern sched_state_t sched;

void sched_reset(void);
void event_repeat(int index, uint64_t ticks);
void sched_update_next_event(void);
void sched_process_pending_events(void);
void event_clear(int index);
void event_set(int index, uint64_t ticks);
uint32_t event_ticks_remaining(int index);
void sched_set_clocks(int count, uint32_t *new_rates);

#ifdef __cplusplus
}
#endif

#endif
