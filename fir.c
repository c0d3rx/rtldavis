#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "ztypes.h"
#include "fir.h"


t_CF32_FIR *new_cf32_fir (float * coeff, unsigned int taps){
  t_CF32_FIR *rt;
  rt = malloc ( sizeof (t_CF32_FIR ));

  rt->coeff= malloc(sizeof(float) * taps);
  memcpy(rt->coeff, coeff, sizeof(float) * taps);
  rt->taps = taps;

  rt->buffer= malloc(sizeof(CF32_t) * taps-1);
  rt->buffer_idx = 0;
  rt->buffer_cnt = 0;

  rt->filter = filter_cf32_fir;


  return rt;
}

CF32_t filter_cf32_fir (t_CF32_FIR *filter, CF32_t in ){
	unsigned int i;
	CF32_t acc={.i=0, .q=0};
	CF32_t prod;
	float *coeffp = filter->coeff;
	unsigned int buffer_idx;
	CF32_t *buffer = filter->buffer;
	buffer_idx = filter->buffer_idx;

	buffer[filter->buffer_idx++] = in;
	filter->buffer_idx %= filter->taps;

	if ( filter->buffer_cnt < filter->taps){
		filter->buffer_cnt++;
		return acc;
	}
	for (i=0; i<filter->taps;i++){
		prod.i = *coeffp * buffer[buffer_idx].i;
		prod.q = *coeffp++ * buffer[buffer_idx].q;

		if (buffer_idx==0)
			buffer_idx = filter->taps-1;
		else
			buffer_idx--;

		acc.i += prod.i;
		acc.q += prod.q;
	}

	return acc;
}


void free_cf32_fir(t_CF32_FIR * filter){
  free (filter);
}
