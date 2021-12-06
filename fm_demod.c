#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "ztypes.h"
#include "fm_demod.h"


t_CF32_FM_DEMOD *new_cf32_fm_demod(float gain, float squelch, unsigned int bias_sample_delay, unsigned int avg_pwr_divider, unsigned int avg_bias_divider){
	t_CF32_FM_DEMOD  *rt;

	rt = malloc ( sizeof ( t_CF32_FM_DEMOD));
	rt->gain = gain;

	rt->last_sample.i = 0;
	rt->last_sample.q = 0;

	rt->squelch = squelch*squelch;		// squared
	rt->squelch_state = 0;

	rt->bias_sample_delay = bias_sample_delay;
	rt->avg_bias_divider = avg_bias_divider;
	rt->avg_pwr_divider = avg_pwr_divider;

	rt->bias_sample_cnt = 0;
	rt->pwr_acc = 0;
	rt->bias_acc = 0;

	rt->demod = demod_cf32_fm_demod;

	return rt;
}


float demod_cf32_fm_demod (t_CF32_FM_DEMOD *demod, CF32_t in ){
	float r=0;
	float rb=0;
	float pwr2;
	CF32_t c;

	pwr2 = cf32_abs2(in);

	demod->pwr_acc-=(demod->pwr_acc/ demod->avg_pwr_divider);
	demod->pwr_acc+=pwr2;

	if (demod->pwr_acc/demod->avg_pwr_divider > demod->squelch){

		if (demod->squelch_state==0){
			demod->bias_sample_cnt = demod->bias_sample_delay;
			demod->squelch_state = 1;
			//printf ("squelch open\n");

		} else {
			if (demod->bias_sample_cnt>0){
				demod->bias_sample_cnt--;
			}
		}

		c = cf32_complex_product(in, cf32_complex_conjugate(demod->last_sample));
		r = atan2f (c.q, c.i) * demod->gain;

		rb = r - demod->bias_acc/demod->avg_bias_divider;
		if ( r < 2 && demod->bias_sample_cnt>0 ){
			demod->bias_acc -= demod->bias_acc/ demod->avg_bias_divider;
			demod->bias_acc += r;
		}

	} else {
		if (demod->squelch_state){
			demod->squelch_state = 0;
			//printf ("squelch closed\n");
			demod->bias_acc=0;
		}

		// demod->bias_acc-=demod->bias_acc / demod->avg_bias_divider;
	}
	demod->last_sample = in;
	return r;
}


void free_cf32_fm_demod(t_CF32_FM_DEMOD * filter){
  free (filter);
}
