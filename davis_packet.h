#ifndef DAVIS_PACKET_H
#define DAVIS_PACKET_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>


#define DAVIS_PACKET_SIZE 10

#define DAVIS_SUPERCAP_VOLTAGE 2
#define DAVIS_UV_INDEX 4
#define DAVIS_RAIN_RATE 5
#define DAVIS_SOLAR_RADIATION  6
#define DAVIS_SOLAR_CELL_OUTPUT  7
#define DAVIS_TEMPERATURE  8
#define DAVIS_WIND_GUST  9
#define DAVIS_HUMIDITY  0x0A
#define DAVIS_RAIN  0x0E
#define DAVIS_SOIL  0x0F

#define STATION_TYPE_VUE 0
#define STATION_TYPE_VP2 1

/**
 *
 */
typedef struct DAVIS_PACKET {
	uint8_t  data[DAVIS_PACKET_SIZE];
	uint16_t station_type;
	uint16_t received_crc;
	uint16_t computed_crc;
	uint8_t  transmitter_id;
	uint8_t  battery_state;
	uint8_t  packet_type;
	float wind_speed;
	float wind_direction;
	float sensor_value;
	float wind_gust;
	uint16_t wind_gust_index;
	char description[64];

	bool valid;
} t_DAVIS_PACKET;


t_DAVIS_PACKET *new_davis_packet(uint8_t *data, uint16_t station_type);
void free_davis_packet (t_DAVIS_PACKET *demod);
char *davis_packet_description(uint8_t);


#endif

