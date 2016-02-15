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

#include "cpu.h"
#include "emu.h"
#include "schedule.h"

sched_state_t sched;

static uint32_t muldiv(uint32_t a, uint32_t b, uint32_t c) {
#if defined(__i386__) || defined(__x86_64__)
    asm ("mull %k1\n\tdivl %k2" : "+a" (a) : "g" (b), "g" (c) : "cc", "edx");
    return a;
#else
    uint64_t d = a;
    d *= b;
    return d / c;
#endif
}

void sched_reset(void) {
    const uint32_t def_rates[] = { 0, 0, 27000000, 12000000, 32768 };
    memcpy(sched.clockRates, def_rates, sizeof(def_rates));
    memset(sched.items, 0, sizeof sched.items);
    sched.nextIndex = 0;
}

void event_repeat(int index, uint64_t ticks) {
    struct sched_item *item = &sched.items[index];

    ticks += item->tick;
    item->second = ticks / sched.clockRates[item->clock];
    item->tick = ticks % sched.clockRates[item->clock];

    item->cputick = muldiv(item->tick, sched.clockRates[CLOCK_CPU], sched.clockRates[item->clock]);
}

void sched_update_next_event(void) {
    int i;
    sched.nextCPUtick = sched.clockRates[CLOCK_CPU];
    sched.nextIndex = -1;

    for(i = 0; i < SCHED_NUM_ITEMS; i++) {
        struct sched_item *item = &sched.items[i];
        if (item->proc != NULL && item->second == 0 && item->cputick < sched.nextCPUtick) {
            sched.nextCPUtick = item->cputick;
            sched.nextIndex = i;
        }
    }
    /* printf("Next event: (%8d,%d)\n", next_cputick, next_index); */
    cpu.next = sched.nextCPUtick;
#ifdef DEBUG_SUPPORT
    if (!cpu.halted && cpu_events & EVENT_DEBUG_STEP) {
        cpu.next = debugger.cpu_cycles + 1;
    }
#endif
}

void sched_process_pending_events(void) {
    sched_update_next_event();
    while (cpu.cycles >= sched.nextCPUtick) {
        if (sched.nextIndex < 0) {
            /* printf("[%8d] New second\n", cputick); */
            int i;
            for (i = 0; i < SCHED_NUM_ITEMS; i++) {
                if (sched.items[i].second >= 0) {
                    sched.items[i].second--;
                }
            }
            cpu.cycles -= sched.clockRates[CLOCK_CPU];
        } else {
            /* printf("[%8d/%8d] Event %d\n", cputick, sched.next_cputick, sched.next_index); */
            sched.items[sched.nextIndex].second = -1;
            sched.items[sched.nextIndex].proc(sched.nextIndex);
        }
        sched_update_next_event();
    }
}

void event_clear(int index) {
    sched_process_pending_events();

    sched.items[index].second = -1;

    sched_update_next_event();
}

void event_set(int index, uint64_t ticks) {
    struct sched_item *item;
    sched_process_pending_events();

    item = &sched.items[index];
    item->tick = muldiv(cpu.cycles, sched.clockRates[item->clock], sched.clockRates[CLOCK_CPU]);
    event_repeat(index, ticks);

    sched_update_next_event();
}

uint64_t event_ticks_remaining(int index) {
    struct sched_item *item;
    sched_process_pending_events();

    item = &sched.items[index];
    return (uint64_t)item->second * sched.clockRates[item->clock]
        + item->tick - muldiv(cpu.cycles, sched.clockRates[item->clock], sched.clockRates[CLOCK_CPU]);
}

void sched_set_clocks(int count, uint32_t *new_rates) {
    int i;
    uint64_t remaining[SCHED_NUM_ITEMS];
    sched_process_pending_events();

    for (i = 0; i < SCHED_NUM_ITEMS; i++) {
        struct sched_item *item = &sched.items[i];
        if (item->second >= 0) {
            remaining[i] = event_ticks_remaining(i);
        }
    }

    cpu.cycles = muldiv(cpu.cycles, new_rates[CLOCK_CPU], sched.clockRates[CLOCK_CPU]);
    memcpy(sched.clockRates, new_rates, sizeof(uint32_t) * count);

    for (i = 0; i < SCHED_NUM_ITEMS; i++) {
        struct sched_item *item = &sched.items[i];
        if (item->second >= 0) {
            item->tick = muldiv(cpu.cycles, sched.clockRates[item->clock], sched.clockRates[CLOCK_CPU]);
            event_repeat(i, remaining[i]);
        }
    }

    sched_update_next_event();
}
