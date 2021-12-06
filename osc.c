#define _OSC_

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "ztypes.h"
#include "osc.h"



t_CF32_OSC *new_cf32_osc(float initial_phase, float phase_inc){
  t_CF32_OSC *rt;
  uint16_t rad;
  float frad=0;

  rt = malloc ( sizeof (t_CF32_OSC ));
  rt->initial_phase = initial_phase;
  rt->current_phase = initial_phase;
  rt->phase_inc = phase_inc;
  rt->next = next_cf32_osc;

  return rt;
}

#define TABLESIZE 6283		// pi*2
#define FRADINC (6.283185307179586/(float)TABLESIZE)

t_CF32_TOSC *new_cf32_tosc(float initial_phase, float phase_inc){
  t_CF32_TOSC *rt;
  uint16_t rad;
  float frad=0;

  rt = malloc ( sizeof (t_CF32_TOSC ));
  rt->initial_phase = (uint16_t) (initial_phase / FRADINC);
  rt->current_phase = rt->initial_phase;
  rt->phase_inc = (uint16_t) (phase_inc / FRADINC);
  rt->next = next_cf32_tosc;

  rt->sine_table = malloc (TABLESIZE * sizeof(float));
  rt->cosine_table = malloc (TABLESIZE * sizeof(float));
  for (rad=0;rad<TABLESIZE;rad++){
	  rt->sine_table[rad] =sinf(frad);
	  rt->cosine_table[rad] = cosf(frad);
	  frad+=FRADINC;
  }

  return rt;
}



CF32_t next_cf32_osc(t_CF32_OSC *osc){
	CF32_t r;

	r.i=sinf(osc->current_phase);
	r.q=-cosf(osc->current_phase);
	// printf ("r.i %f r.q %f\n", r.i, r.q);

	osc->current_phase+=osc->phase_inc;
	if (osc->current_phase >= 2*M_PI) osc->current_phase-=(2*M_PI);
	return r;

}

CF32_t next_cf32_tosc(t_CF32_TOSC *osc){
	CF32_t r;


	r.i = osc->sine_table[osc->current_phase];
	r.q = -osc->cosine_table[osc->current_phase];

	osc->current_phase+=osc->phase_inc;
	if (osc->current_phase >= TABLESIZE) osc->current_phase-=TABLESIZE;
	return r;

}



void free_cf32_osc(t_CF32_OSC * r){
  free (r);
}

void free_cf32_tosc(t_CF32_TOSC * r){
  free (r);
}


float get_phase_inc (float frq, uint32_t samp_rate){
	float phase_inc = frq * (2*M_PI) / (float) samp_rate;
	return phase_inc;
}



