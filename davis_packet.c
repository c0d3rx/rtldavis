#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include "ztypes.h"
#include "davis_packet.h"

#define WTROWS 55
#define WTCOLS 35
static int16_t windtab[WTROWS][WTCOLS]={
        {0, 1, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 127, 128},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},
        {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},
        {5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},
        {6, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0},
        {7, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 0, 0},
        {8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 0, 0},
        {9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 0, 0},
        {10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 0, 0},
        {11, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 0, 0},
        {12, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 0, 0},
        {13, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 3, 1, 0, 0},
        {14, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 3, 1, 0, 0},
        {15, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 3, 1, 0, 0},
        {16, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 3, 1, 0, 0},
        {17, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 3, 1, 0, 0},
        {18, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 3, 1, 0, 0},
        {19, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 4, 4, 1, 0, 0},
        {20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 3, 4, 4, 2, 0, 0},
        {21, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 3, 4, 4, 2, 0, 0},
        {22, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 3, 4, 4, 2, 0, 0},
        {23, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 3, 4, 4, 2, 0, 0},
        {24, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 3, 4, 4, 2, 0, 0},
        {25, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 3, 4, 4, 2, 0, 0},
        {26, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 3, 5, 4, 2, 0, 0},
        {27, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 3, 5, 5, 2, 0, 0},
        {28, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 3, 5, 5, 2, 0, 0},
        {29, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 3, 5, 5, 2, 0, 0},
        {30, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 3, 5, 5, 2, 0, 0},
        {35, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 4, 6, 5, 2, 0, -1},
        {40, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 4, 6, 6, 2, 0, -1},
        {45, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 4, 7, 6, 2, -1, -1},
        {50, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 5, 7, 7, 2, -1, -2},
        {55, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 5, 8, 7, 2, -1, -2},
        {60, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 5, 8, 8, 2, -1, -2},
        {65, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 5, 9, 8, 2, -2, -3},
        {70, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 0, 2, 5, 9, 9, 2, -2, -3},
        {75, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 0, 2, 6, 10, 9, 2, -2, -3},
        {80, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 0, 2, 6, 10, 10, 2, -2, -3},
        {85, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 0, 2, 7, 11, 11, 2, -3, -4},
        {90, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 7, 12, 11, 2, -3, -4},
        {95, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 2, 2, 3, 2, 2, 2, 1, 1, 1, 1, 2, 7, 12, 12, 3, -3, -4},
        {100, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 2, 3, 3, 2, 2, 2, 1, 1, 1, 1, 2, 8, 13, 12, 3, -3, -4},
        {105, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 3, 3, 3, 2, 2, 2, 1, 1, 1, 2, 8, 13, 13, 3, -3, -4},
        {110, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 3, 3, 3, 2, 2, 2, 1, 1, 1, 2, 8, 14, 14, 3, -3, -5},
        {115, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 2, 2, 2, 1, 1, 1, 2, 9, 15, 14, 3, -3, -5},
        {120, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 2, 2, 2, 1, 1, 1, 3, 9, 15, 15, 3, -4, -5},
        {125, 1, 1, 2, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 2, 2, 1, 1, 1, 3, 10, 16, 16, 3, -4, -5},
        {130, 1, 1, 2, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 1, 1, 3, 10, 17, 16, 3, -4, -6},
        {135, 1, 2, 2, 1, 1, 0, 0, 0, -1, 0, 0, 1, 1, 2, 2, 3, 3, 3, 3, 4, 3, 3, 2, 2, 2, 1, 1, 3, 10, 17, 17, 4, -4, -6},
        {140, 1, 2, 2, 1, 1, 0, 0, 0, -1, 0, 0, 1, 1, 2, 2, 3, 3, 3, 4, 4, 3, 3, 2, 2, 2, 1, 1, 3, 11, 18, 17, 4, -4, -6},
        {145, 2, 2, 2, 1, 1, 0, 0, 0, -1, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 4, 3, 3, 3, 2, 2, 1, 1, 3, 11, 19, 18, 4, -4, -6},
        {150, 2, 2, 2, 1, 1, 0, 0, -1, -1, 0, 0, 1, 1, 2, 3, 3, 4, 4, 4, 4, 4, 3, 3, 2, 2, 1, 1, 3, 12, 19, 19, 4, -4, -6}
    };


float interpolate2 (float rx0, float rx1, float ry0, float ry1, float x0, float x1, float y0, float y1, float x, float y){
	float dy0, dy1;

	if (rx0 == rx1)
		return y + x0 + (y - ry0) / (ry1 - ry0) * (y1 - y0);

	if (ry0 == ry1)
	            return y + y0 + (x - rx0) / (rx1 - rx0) * (x1 - x0);


    dy0 = x0 + (y - ry0) / (ry1 - ry0) * (y0 - x0);
    dy1 = x1 + (y - ry0) / (ry1 - ry0) * (y1 - x1);

    return y + dy0 + (x - rx0) / (rx1 - rx0) * (dy1 - dy0);


}


float calc_wind_speed(int16_t raw_mph, int16_t raw_angle){
	uint16_t s0,s1,a0,a1;
	if (raw_angle>128){
		raw_angle = 256 - raw_angle;
	}
	s0=1;a0=1;
	while (windtab[s0][0] < raw_mph) s0++;
	while (windtab[0][a0] < raw_angle) a0++;
	if (windtab[s0][0] == raw_mph) {
		s1 = s0;
	} else {
		if (s0>1){
			s0--;
		}
		if (s0 == WTROWS-1 ){
			s1 = WTROWS-1;
		} else {
			s1 = s0 + 1;
		}
	}
	if (windtab[0][a0] == raw_angle) {
		a1 = a0;
	} else {
		if (a0>1){
			a0--;
		}
		if (a0 == WTCOLS-1 ){
			a1 = WTCOLS-2;
		} else {
			a1 = a0 + 1;
		}
	}
	if ((s0==s1) && (a0==a1)){
		return (float) (raw_mph + windtab[s0][a0]);
	}
	return interpolate2((float)windtab[0][a0], (float)windtab[0][a1],
			(float)windtab[s0][0], (float)windtab[s1][0],
			(float)windtab[s0][a0], (float)windtab[s0][a1],
			(float)windtab[s1][a0], (float)windtab[s1][a1],
			(float)raw_angle, (float)raw_mph);

}



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
			float ecspd;
			ecspd = calc_wind_speed((int16_t)data[1], (int16_t)data[2]);
			//packet->wind_speed = (float)data[1] * 1.609344;
			packet->wind_speed = ecspd * 1.609344;

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




