#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include "ztypes.h"
#include "davis_packet.h"


uint16_t crc16_ccitt( const void *buf, int len ){
	unsigned short crc = 0;
	while( len-- ) {
		int i;
		crc ^= *(char *)buf++ << 8;
		for( i = 0; i < 8; ++i ) {
			if( crc & 0x8000 )
				crc = (crc << 1) ^ 0x1021;
			else
				crc = crc << 1;
		}
	}
	return crc;
}


t_DAVIS_PACKET *new_davis_packet(uint8_t *data, uint16_t station_type){
	t_DAVIS_PACKET  *packet;
	int i;

	/*
	for (i=0;i<DAVIS_PACKET_SIZE;i++){
		printf ("%.02x ",data[i]);
	}
	printf ("\n");*/


	packet = malloc ( sizeof ( t_DAVIS_PACKET));
	memcpy(packet->data, data, DAVIS_PACKET_SIZE);

	packet->station_type = station_type;
	packet->sensor_value = FLT_MIN;

	packet->received_crc = (data[6]<<8) | data[7];
	packet->computed_crc = crc16_ccitt(data, 6);
	packet->valid = packet->received_crc == packet->computed_crc;

//	printf ("received_crc %.4x\n",packet->received_crc);
//	printf ("computed_crc %.4x\n",packet->computed_crc);

	packet->transmitter_id = data[0] & 0b111;
	packet->battery_state = (data[0] & 0b0001000) >> 3;

	packet->packet_type = data[0]>>4;
//	printf ("packet_type: %d\n", packet->packet_type);

	switch (packet->packet_type){
	case DAVIS_HUMIDITY: 	strcpy(packet->description,"humidity");break;
	case DAVIS_RAIN: 		strcpy(packet->description,"rain-counter");break;
	case DAVIS_RAIN_RATE:  	strcpy(packet->description,"rain-rate");break;
	case DAVIS_SOIL: 		strcpy(packet->description, "soil");break;
	case DAVIS_SOLAR_CELL_OUTPUT: strcpy(packet->description,"solar-cell-output");break;
	case DAVIS_SOLAR_RADIATION: strcpy(packet->description,"solar-radiation");break;
	case DAVIS_TEMPERATURE: strcpy(packet->description,"temperature");break;
	case DAVIS_WIND_GUST: strcpy(packet->description,"wind-gust");break;
	case DAVIS_UV_INDEX: strcpy(packet->description,"uv-index");break;
	case DAVIS_SUPERCAP_VOLTAGE: strcpy(packet->description,"supercap-voltage");break;
	default:sprintf (packet->description, "type_%.2u", packet->packet_type);break;
	}




	if (packet->packet_type!=DAVIS_SOIL){
		packet->wind_speed = -1;
		if (data[1]!=0xff){
			packet->wind_speed = (float)data[1] * 1.609344;
		}
//		printf ("wind_speed: %f\n", packet->wind_speed);

		packet->wind_direction=-1;
		if (station_type==STATION_TYPE_VUE){
			if (data[2]!=0xff){
				packet->wind_direction=(float)data[2] * 1.40625 + 0.3;
			} else {
				packet->wind_direction = 0;
			}
		} else
		if (station_type==STATION_TYPE_VP2){
			if (data[2]!=0xff){
				packet->wind_direction=9 + ((float)data[2] * 342.)/255.;
			} else {
				packet->wind_direction = 0;
			}
		}
//		printf ("wind_direction: %f\n", packet->wind_direction);

		if (packet->packet_type==DAVIS_SUPERCAP_VOLTAGE){
			packet->sensor_value = (float)((data[3]<<2)|((data[4] & 0b11000000)>>6)) / 100.;
		} else
		if (packet->packet_type==DAVIS_UV_INDEX){
			packet->sensor_value = (float) (((data[3] << 8) + data[4]) >> 6) / 50.;
		} else
		if (packet->packet_type==DAVIS_RAIN_RATE){
			if (data[3]!=0xff){
				if (data[4] & 0x40){ // strong rain
					packet->sensor_value = 720. / (float)(((data[4] & 0x30) / 16 * 250) + data[3]); // mm/h
				} else {
					packet->sensor_value = 11520. / (float)(((data[4] & 0x30) / 16 * 250) + data[3]);

				}
			}
		} else
		if (packet->packet_type==DAVIS_TEMPERATURE){
			int16_t far16;
			float far;
			float deg;
			far16 = (((data[3] << 8) | data[4]));
			far = ((float) far16) / 160.;
			deg = (far-32) * 5. / 9.;
			packet->sensor_value = deg;
		} else
		if (packet->packet_type==DAVIS_HUMIDITY){
			packet->sensor_value = (float)(((data[4] >> 4) << 8) + data[3]) / 10.0;
		} else
		if (packet->packet_type==DAVIS_SOLAR_RADIATION){
			packet->sensor_value = (float)(((data[3] << 8) + data[4]) >> 6) * 1.757936;
		} else
		if (packet->packet_type==DAVIS_RAIN){
			//if (data[3]!=0x80){
				packet->sensor_value = (float) data[3];
			//}
		} else
		if (packet->packet_type==DAVIS_SOLAR_CELL_OUTPUT){
			packet->sensor_value = (float)((data[3]<<2)|((data[4] & 0b11000000)>>6)) / 100.;
		} else
		if (packet->packet_type==DAVIS_WIND_GUST){
			packet->wind_gust = (float) data[3] * 1.609344;
			packet->wind_gust_index = data[4]>>4;
		}

	}

//	printf ("packet_type %d, sensor_value %f\n",packet->packet_type, packet->sensor_value);

	return packet;
}


void free_davis_packet(t_DAVIS_PACKET *packet){
  free (packet);
}




