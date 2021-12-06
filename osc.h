#ifndef OSC_H
#define OSC_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>


typedef struct CF32_OSC {
  float initial_phase;
  float phase_inc;
  float current_phase;
  CF32_t (*next) (struct CF32_OSC *osc);
} t_CF32_OSC;

typedef struct CF32_TOSC {
  float initial_phase;
  uint16_t phase_inc;
  uint16_t current_phase;

  float *sine_table;
  float *cosine_table;

  CF32_t (*next) (struct CF32_TOSC *osc);
} t_CF32_TOSC;





t_CF32_OSC *new_cf32_osc(float initial_phase, float phase_inc);
t_CF32_TOSC *new_cf32_tosc(float initial_phase, float phase_inc);

CF32_t next_cf32_osc(t_CF32_OSC *r);
CF32_t next_cf32_tosc(t_CF32_TOSC *r);

void free_cf32_osc(t_CF32_OSC * r);
void free_cf32_tosc(t_CF32_TOSC * r);

float get_phase_inc (float, uint32_t);


#endif

