// C Standard Library (POSIX)
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <stdarg.h>
#include <getopt.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#include "rtl-sdr.h"
#include "convenience.h"

#include "rtlsdr_reader.h"


float default_sdr_gain=43;
int default_sdr_ppm_error;
int dev_index = 0;

extern void usage(void);
void rtlsdr_reader_usage(void) {
	usage();
	fprintf(stderr, "\t[--gain=<host> (default %f)]\n"
			"\t[--dev-index=<host> (default %d)]\n"
			"\t--ppm-error=<ppm error> (default %d)\n\n",default_sdr_gain,verbose_device_search("0"),default_sdr_ppm_error);
	exit(1);
}


t_RTLSDR_READER *new_rtlsdr_from_args(int argc, char **argv, unsigned int buf_len, void (*callback)(struct RTLSDR_READER *, uint8_t *, unsigned int)){


	static struct option long_options[] = {
			{ "gain", required_argument,	NULL, 0 },
			{ "ppm-error", required_argument, NULL, 0 },
			{ "dev_index", required_argument, NULL, 0 },
			{ NULL, 0,	NULL, 0 } };
	int c;
	int option_index = 0;



	int dev_given=0 ;

	float gain=default_sdr_gain;
	float ppm_error = default_sdr_ppm_error;

	optind = 0;
	while ((c = getopt_long(argc, argv, ":h", long_options, &option_index)) != -1) {
		switch (c) {
		case 0:
			switch (option_index) {
			case 0:
				gain = (atof(optarg));
				break;
			case 1:
				ppm_error = atoi(optarg);
				break;
			case 2:
				dev_index = verbose_device_search(optarg);
				dev_given=1;
				break;
			}
			break;
		case 'h':
			rtlsdr_reader_usage();
			break;
		}
	}
	if (!dev_given) {
		dev_index = verbose_device_search("0");
	}

	return new_rtlsdr(dev_index, buf_len, ppm_error, gain, callback);

}

/**
 * todo: aggiungere ppm, freq, gain ( opzionali )
 */

t_RTLSDR_READER  *new_rtlsdr(int dev_index , unsigned int buf_len, int ppm_error, float gain, void (*callback)(t_RTLSDR_READER *, uint8_t *, unsigned int)){
	t_RTLSDR_READER *rt = malloc (sizeof(t_RTLSDR_READER));
	int r;

	rt->buffer = malloc (buf_len);
	rt->buffer_len = buf_len;
	rt->dev_index = dev_index;
	r = rtlsdr_open(&rt->dev, (uint32_t) dev_index);
	if (r < 0) {
		fprintf(stderr, "Failed to open rtlsdr device #%d.\n", dev_index);
		exit(1);
	}
	verbose_reset_buffer(rt->dev);

	set_gain_rtlsdr(rt, gain);
	set_ppm_error_rtlsdr(rt, ppm_error);


    rt->read_chunk = read_rtlsdr_chunk;
    rt->callback = callback;
    rt->set_frequency = set_frequency_rtlsdr;
    rt->set_gain = set_gain_rtlsdr;
    rt->set_sample_rate = set_sample_rate_rtlsdr;

    return rt;
}


void free_rtlsdr(t_RTLSDR_READER * rtlsdr){
	rtlsdr_cancel_async(rtlsdr->dev);
    free(rtlsdr);
}

int set_frequency_rtlsdr(t_RTLSDR_READER *rtlsdr, int freq){
	return verbose_set_frequency(rtlsdr->dev, freq);
}

int set_sample_rate_rtlsdr(t_RTLSDR_READER *rtlsdr, int samp_rate){
	return verbose_set_sample_rate(rtlsdr->dev, samp_rate);
}
int set_ppm_error_rtlsdr(t_RTLSDR_READER *rtlsdr, int ppm_error){

	return verbose_ppm_set(rtlsdr->dev, ppm_error);
}


int set_gain_rtlsdr(t_RTLSDR_READER *rtlsdr, float gain){
	int gain10;
	gain10 = (int)(gain*10);
	gain10 = nearest_gain(rtlsdr->dev, gain10);
	return verbose_gain_set(rtlsdr->dev, gain10);
}

int read_rtlsdr_chunk(t_RTLSDR_READER * rtlsdr){
	// callback....
	int r;
	int n_read;
	r = rtlsdr_read_sync(rtlsdr->dev, rtlsdr->buffer, rtlsdr->buffer_len, &n_read);
	rtlsdr->callback(rtlsdr, rtlsdr->buffer, rtlsdr->buffer_len);
    return r;
}
