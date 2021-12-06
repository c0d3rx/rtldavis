#ifndef FIR_H_
#define FIR_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>


typedef struct CF32_FIR {
	float *coeff;
	unsigned int taps;

	CF32_t *buffer;
	unsigned int buffer_idx;		// ring buffer index
	unsigned int buffer_cnt;
	CF32_t (*filter)(struct CF32_FIR *, CF32_t);

} t_CF32_FIR;


t_CF32_FIR *new_cf32_fir(float *coeff, unsigned int taps);
CF32_t filter_cf32_fir (t_CF32_FIR *filter, CF32_t in );
void free_cf32_fir(t_CF32_FIR *filter);


#endif

