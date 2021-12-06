#ifndef FSK_DEMOD_H_
#define FSK_DEMOD_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/**
 *
 */
typedef struct F32_FSK_DEMOD {

	uint16_t samples_per_symbol;
	uint16_t phase;

	float last_sample;
	uint16_t lastbit;

	void (*demod)(struct F32_FSK_DEMOD *, float);
	void (*callback)(struct F32_FSK_DEMOD *, uint16_t bit);

} t_F32_FSK_DEMOD;


t_F32_FSK_DEMOD *new_cf32_fsk_demod(uint16_t samples_per_symbol, void (*callback)(t_F32_FSK_DEMOD *, uint16_t ));
void demod_cf32_fsk_demod (t_F32_FSK_DEMOD *demod, float in );
void free_cf32_fsk_demod (t_F32_FSK_DEMOD *demod);


#endif
