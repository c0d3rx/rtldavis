#ifndef FM_DEMOD_H_
 #define FM_DEMOD_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>


/**
 */
typedef struct CF32_FM_DEMOD {

	float gain;
	CF32_t last_sample;

	float squelch;
	uint16_t squelch_state;


	unsigned int avg_pwr_divider;
	unsigned int avg_bias_divider;
	unsigned int bias_sample_delay;

	unsigned int bias_sample_cnt;
	float pwr_acc;
	float bias_acc;

	float (*demod)(struct CF32_FM_DEMOD *, CF32_t);

} t_CF32_FM_DEMOD;


t_CF32_FM_DEMOD *new_cf32_fm_demod(float gain, float squelch, unsigned int bias_sample_delay, unsigned int avg_pwr_divider, unsigned int avg_bias_divider);
float demod_cf32_fm_demod (t_CF32_FM_DEMOD *, CF32_t in );
void free_cf32_fm_demod(t_CF32_FM_DEMOD *);


#endif

