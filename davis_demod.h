#ifndef DAVIS_DEMOD_H_
#define DAVIS_DEMOD_H_

enum it_STATE{
	WAIT_FOR_SYNC=0,
	PACKET_SAMPLING
};


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define SYNCWORD 0xaaaacb89


/**
 *
 */
typedef struct DAVIS_DEMOD {

	uint8_t bit_mask;
	uint16_t byte_index;
	uint8_t data[DAVIS_PACKET_SIZE];

	uint16_t state;
	uint32_t sync_buffer;

	void (*feed)(struct DAVIS_DEMOD *, uint16_t);						// bit feed
	void (*callback)(struct DAVIS_DEMOD *, t_DAVIS_PACKET *packet);	// callback, when successfully decoded a packet

} t_DAVIS_DEMOD;

t_DAVIS_DEMOD *new_davis_demod(void (*callback)(t_DAVIS_DEMOD *, t_DAVIS_PACKET * )); // pacchetto davis
void feed_davis_demod (t_DAVIS_DEMOD *demod, uint16_t bit );		// bit feed
void free_davis_demod (t_DAVIS_DEMOD *demod);


#endif

