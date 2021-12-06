#define _DAVIS_DEMOD_

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "ztypes.h"
#include "davis_packet.h"
#include "davis_demod.h"



t_DAVIS_DEMOD *new_davis_demod(void (*callback)(t_DAVIS_DEMOD *, t_DAVIS_PACKET *)){
	t_DAVIS_DEMOD  *demod;

	demod = malloc ( sizeof ( t_DAVIS_DEMOD));
	demod->callback = callback;
	demod->feed = feed_davis_demod;
	demod->state = WAIT_FOR_SYNC;

	return demod;
}

void feed_davis_demod (t_DAVIS_DEMOD *demod, uint16_t bit){
	int i;
	t_DAVIS_PACKET *packet;
	switch (demod->state){
	case WAIT_FOR_SYNC:
		// fill sync buffer
		demod->sync_buffer = (demod->sync_buffer<<1) | bit;
		if (demod->sync_buffer==SYNCWORD){
			demod->state = PACKET_SAMPLING;
			//printf ("got sync\n");
			demod->byte_index=0;
			demod->bit_mask=1;
			memset(demod->data, 0, DAVIS_PACKET_SIZE);
		}
		break;
	case PACKET_SAMPLING:
		if (bit){
			demod->data[demod->byte_index] |= demod->bit_mask;
		}
		demod->bit_mask<<=1;
		if (demod->bit_mask==0){
			demod->bit_mask = 1;
			demod->byte_index++;
			if (demod->byte_index==DAVIS_PACKET_SIZE) {
				//printf ("GOT PACKET!!");
				packet = new_davis_packet(demod->data,STATION_TYPE_VUE);
				// call callback
				demod->callback(demod, packet);
				demod->state= WAIT_FOR_SYNC;
			}
		}
		break;
	}
}

void free_davis_demod(t_DAVIS_DEMOD * demod){
  free (demod);
}
