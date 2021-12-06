#define _FSK_DEMOD_

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "ztypes.h"
#include "fsk_demod.h"


t_F32_FSK_DEMOD *new_cf32_fsk_demod(uint16_t samples_per_symbol, void (*callback)(t_F32_FSK_DEMOD *, uint16_t )){
	t_F32_FSK_DEMOD  *demod;

	demod = malloc ( sizeof ( t_F32_FSK_DEMOD));
	demod->samples_per_symbol = samples_per_symbol;

	demod->callback = callback;
	demod->demod = demod_cf32_fsk_demod;

	demod->phase=demod->samples_per_symbol/2;	//sampling phase
	demod->last_sample = 0;

	demod->lastbit = 0;

	return demod;
}


/**
 * return 0,1 or -1 -1=no bit for this sample
 */
void demod_cf32_fsk_demod (t_F32_FSK_DEMOD *demod, float sample ){

	uint16_t bit = demod->lastbit;
	// check for transition to sync bit sampler
	if (  (sample>=0 && demod->last_sample<0) || (sample < 0 && demod->last_sample>=0) ){
		if (demod->phase >demod->samples_per_symbol/2 ){
			demod->phase--;
		}
		if (demod->phase < demod->samples_per_symbol/2 ){
			demod->phase++;
		}
	}
	demod->last_sample = sample;
	demod->phase++;
	demod->phase %= demod->samples_per_symbol;

	if (demod->phase == 0){
		bit = sample>0?1:0;
		demod->callback(demod, bit);
	}

}


void free_cf32_fsk_demod(t_F32_FSK_DEMOD * demod){
  free (demod);
}
