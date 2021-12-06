#ifndef RTLSDR_READER_H_
#define RTLSDR_READER_H_

#include <rtl-sdr.h>

typedef struct RTLSDR_READER{
	rtlsdr_dev_t *dev;
	uint8_t *buffer;
	int dev_index;
	unsigned int buffer_len;

	void (*callback)(struct RTLSDR_READER *, uint8_t *, unsigned int);
	int  (*read_chunk)(struct RTLSDR_READER *);
	int  (*set_frequency)(struct RTLSDR_READER *,  int);
	int  (*set_gain)(struct RTLSDR_READER *,  float);
	int  (*set_sample_rate)(struct RTLSDR_READER *,  int);

} t_RTLSDR_READER;


t_RTLSDR_READER *new_rtlsdr(int, unsigned int, int, float, void (*)(t_RTLSDR_READER *, uint8_t *, unsigned int));
t_RTLSDR_READER *new_rtlsdr_from_args(int, char **, unsigned int, void (*)(struct RTLSDR_READER *, uint8_t *, unsigned int));


int set_frequency_rtlsdr(t_RTLSDR_READER *, int);
int set_gain_rtlsdr(t_RTLSDR_READER *, float);
int set_sample_rate_rtlsdr(t_RTLSDR_READER *, int);
int set_ppm_error_rtlsdr(t_RTLSDR_READER *, int );

int read_rtlsdr_chunk(t_RTLSDR_READER *);
void free_rtlsdr(t_RTLSDR_READER *);

#endif
