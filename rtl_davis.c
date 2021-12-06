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

#include "rtl-sdr.h"
#include "convenience.h"

#include "globals.h"
#include "ztypes.h"
#include "sample_writer.h"
#include "sample_reader.h"
#include "rtltcp_reader.h"
#include "rtlsdr_reader.h"
#include "osc.h"
#include "fir.h"
#include "fm_demod.h"
#include "fsk_demod.h"
#include "davis_packet.h"
#include "davis_demod.h"


#define DEFAULT_BUF_LENGTH		(2 * 16384)
#define MINIMAL_BUF_LENGTH		512
#define MAXIMAL_BUF_LENGTH		(256 * 16384)

#define FSK_DEVIATION 48000
#define BAUD_RATE 19200

float coeff[] = { 0.00092894, 0.02118765, 0.10385019, 0.22853318, 0.29100009,
		0.22853318, 0.10385019, 0.02118765, 0.00092894 };
float fm_demod_gain;
unsigned int bias_sample_delay;

#define SOURCE_FILE 0
#define SOURCE_TCP 1
#define SOURCE_RTL 2


int verbose = 0;
int freq = 868012345;
int samp_rate = 249600;

int source;


int davis_frq[]= {868077250, 868197250, 868317250, 868437250, 868557250};
int davis_hop[]= {0,2,4,1,3};
int davis_hop_index = 0;


//int base_shift = -24950;
int base_shift = 0;

int frq_offs = 40000; 	// ... for +40K
float sqlevel = -15; 	// fm decoder squelch

int hopper_status=0;
int hopper_resync=0;
uint64_t hopper_next_time;


/** interface functions **/
int (*chunk_reader)(void);

int (*set_frequency)(int);
int (*set_sample_rate)(int);
int (*set_gain)(float);


t_SAMPLEWRITER *samplewriter;
t_SAMPLEREADER *samplereader;
t_RTLTCPREADER *tcp_reader;
t_RTLSDR_READER *sdr_reader;


t_CF32_TOSC *oscillator;
t_CF32_FIR *fir;

t_CF32_FM_DEMOD *fm_demod;
t_F32_FSK_DEMOD *fsk_demod;
t_DAVIS_DEMOD *davis_demod;

static int do_exit = 0;


char *dump_file="/tmp/cui8_249600.raw";
bool dump_on_file=false;
void usage(void) {
	fprintf(stderr,
			"\nrtl_davis, davis protocol decoder based on DVB-T receivers\n\n"
			 "Usage:\t --source=[rtl|rtl_tcp|file] (default rtl)\n"
			"\t[--squelch-level=<fm demodulator squelch level>(default %f)]\n"
			"\t[--freq-shift=<freq shift>(default %d)]\n"
			"\t[--dump-on-file=<sample dump file for debugging> (default %s)]\n",sqlevel, base_shift, dump_file);
}

static void sighandler(int signum){
	fprintf(stderr, "Signal caught, exiting!\n");
	do_exit = 1;
}


void davis_callback(t_DAVIS_DEMOD *demod, t_DAVIS_PACKET *packet) {
	//static char tgmt[40], tlocal[40];

	//times_iso8601(tgmt, tlocal);
	if (packet->valid){

		printf ("{\"time_stamp\": %ld, \"valid\": true, \"battery_state\": \"%s\", \"wind_speed\": %f, \"wind_dir\": %f, \"%s\": %f}\n",
				millis(),
				packet->battery_state?"low":"ok",
				packet->wind_speed,
				packet->wind_direction,
				packet->description,
				packet->sensor_value);
		fflush(stdout);
		/*
		printf ("battery-state: %s\n",packet->battery_state?"low":"ok");
		printf ("wind_speed: %f\n", packet->wind_speed);
		printf ("wind_direction: %f\n", packet->wind_direction);
		printf ("%.20s: %f\n", packet->description, packet->sensor_value);
		if (packet->packet_type==DAVIS_WIND_GUST){
			printf ("wind_gust_index %d; wind_gust_value: %f\n", packet->wind_gust_index,packet->wind_gust);
		}*/
	}else {
		printf ("{\"time_stamp\": %ld, \"valid\": false, \"received_crc\": \"%.4x\", \"computed_crc\": \"%.4x\"}\n",
				millis(),
				packet->received_crc,
				packet->computed_crc);
		fflush(stdout);
	}

	// sync hop in 200 msec
	hopper_status=1;
	hopper_resync= 9;
	hopper_next_time = millis()+500;

	// set hop frq changer
}

void fsk_callback(t_F32_FSK_DEMOD *demod, uint16_t bit) {
	davis_demod->feed(davis_demod, bit);
}

/**
 * unified decoder
 */
//#define _PROFILE
void decoder(CF32_t *sample, unsigned int cnt){
	unsigned int i;
	CF32_t m;
	float fm_sample;

#ifdef _PROFILE
	uint64_t start_time;
	printf ("got %u samples\n", cnt/2);
	start_time = micros();
#endif

	if (millis()>hopper_next_time){
		davis_hop_index++;
		davis_hop_index%=(sizeof(davis_hop)/sizeof(int));

		freq = davis_frq[davis_hop[davis_hop_index]] + base_shift + frq_offs ;
		set_frequency(freq);

		if (hopper_resync>0) hopper_resync--;
		if (hopper_resync==0){
			hopper_status=0;
		}
		if (hopper_status){
			hopper_next_time = millis()+2500;
		} else {
			hopper_next_time = millis()+25000;
		}

	}

	for (i=0; i< cnt; i++){
		m = cf32_complex_product(sample[i], oscillator->next(oscillator));
		m = fir->filter(fir, m);

		fm_sample = fm_demod->demod(fm_demod,m);

		if (fm_demod->squelch_state == 1){
			fsk_demod->demod(fsk_demod, fm_sample);
		}
	}

#ifdef _PROFILE
	printf ("%ld usec\n", (micros()-start_time));
#endif

}

CF32_t *cf32buf;

/**
 * rtl sdr source
 */
void rtl_sdr_callback(t_RTLSDR_READER *reader, uint8_t *buf, uint32_t len){
	CF32_t sample, *dst=cf32buf;
	uint32_t slen;
	//printf ("got %d samples from tcpreader...\n", len);
	if (dump_on_file){
		samplewriter->write(samplewriter, buf, S_UI8, len);
	}

	len/=2;
	slen=len;
	//printf ("Got %u complex from rtl\n",len);
	while (len--){
        sample.i=(float)(*buf++ -128)/127.0;
        sample.q=(float)(*buf++ -128)/127.0;
        *dst++ = sample;
	}
	decoder (cf32buf, slen);
}
int rtl_sdr_chunk_reader(void){
	return sdr_reader->read_chunk(sdr_reader);
}

int rtl_sdr_source_set_frequency(int f){
	if (verbose)
		printf ("Set frequency [%d]\n", f);
	return sdr_reader->set_frequency(sdr_reader, f);
}

int rtl_sdr_source_set_sample_rate(int sr){
	if (verbose)
		printf ("Set sample rate [%d]\n", sr);
	return sdr_reader->set_sample_rate(sdr_reader, sr);
}

int rtl_sdr_source_set_gain(float g){
	if (verbose)
		printf ("Set gain [%f]\n", g);
	return sdr_reader->set_gain(sdr_reader, g);
}

/**
 * rtl_tcp source
 */
void rtl_tcp_callback(t_RTLTCPREADER *reader, uint8_t *buf, uint32_t len){
	CF32_t sample, *dst=cf32buf;
	uint32_t slen;
	//printf ("got %d samples from tcpreader...\n", len);
	if (dump_on_file){
		samplewriter->write(samplewriter, buf, S_UI8, len);
	}
	len/=2;
	slen=len;
	//printf ("Got %u complex from rtl\n",len);
	while (len--){
        sample.i=(float)(*buf++ -128)/127.0;
        sample.q=(float)(*buf++ -128)/127.0;
        *dst++ = sample;
	}
	decoder (cf32buf, slen);
}
int rtl_tcp_chunk_reader(void){
	return tcp_reader->read_chunk(tcp_reader);
}
int rtl_tcp_source_set_frequency(int f){
	if (verbose)
		printf ("Set frequency [%d]\n", f);
	return tcp_reader->command(tcp_reader, RTLTCP_SET_FREQ, f);
}

int rtl_tcp_source_set_sample_rate(int sr){
	if (verbose)
		printf ("Set sample rate [%d]\n", sr);
	return tcp_reader->command(tcp_reader, RTLTCP_SET_SAMPLE_RATE, sr);
}

int rtl_tcp_source_set_gain(float g){
	if (verbose)
		printf ("Set gain [%f]\n", g);
	return tcp_reader->command(tcp_reader, RTLTCP_SET_GAIN, (int)(g*10));
}

/**
 * file source
 */
void source_file_callback(t_SAMPLEREADER *sr, CF32_t *sample, unsigned int cnt){
	//printf ("got %d samples from samplereader...\n", cnt);
	decoder (sample, cnt);
}
int source_file_chunk_reader(void){
	return samplereader->read_chunk(samplereader);
}

int source_file_set_frequency(int f){
	if (verbose)
		printf ("Set frequency [%d] not implemented in file source\n", f);
	return 0;
}

int source_file_set_sample_rate(int sr){
	if (verbose)
		printf ("Set sample rate [%d] not implemented in file source\n", sr);
	return 0;
}

int source_file_set_gain(float g){
	if (verbose)
		printf ("Set gain [%f] not implemented in file source\n", g);
	return 0;
}

int main(int argc, char **argv) {
	struct sigaction sigact;
	uint8_t *buffer;

	static struct option long_options[] = {
			{ "source",	required_argument, NULL, 0 },
			{ "squelch-level",	required_argument, NULL, 0 },
			{ "dump-on-file",	optional_argument, NULL, 0 },
			{ "freq-shift",	required_argument, NULL, 0 },
			{ NULL, 0, NULL, 0 } };

	int c;
	int option_index = 0;
	int dev_index = 0;
	int r;

	source = SOURCE_RTL;

	while ((c = getopt_long(argc, argv, ":hv", long_options, &option_index)) != -1) {
		switch (c) {
		case 0:
			switch (option_index) {
			case 0:
				if(!strcmp(optarg,"rtl")){
					source = SOURCE_RTL;
				} else
				if(!strcmp(optarg,"file")){
					source = SOURCE_FILE;
				} else
				if (!strcmp(optarg,"rtl_tcp")){
					source = SOURCE_TCP;
				} else {usage(); exit(1);}
				break;

			case 1: // squelch-level
				sqlevel = atof (optarg);
				break;
			case 2: // dump-on-file
				dump_on_file = true;
				if (optarg){
					dump_file = malloc (1+strlen(optarg));
					strcpy(dump_file, optarg);
				}

				break;
			case 3: // base shift
				base_shift = atoi (optarg);
			}
			break;
		case 'v':
			verbose++;
			break;
		case 'h':
			if (source == -1){
				usage();
				exit(1);
			}
			break;
		}
	}

	switch (source){
		case SOURCE_FILE:
			samplereader = new_samplereader_from_args(argc, argv, S_CF32, DEFAULT_BUF_LENGTH, (void (*)(t_SAMPLEREADER *, void *, unsigned int)) source_file_callback);
			if (samplereader==NULL) exit(1);
			fprintf (stderr, "reading sample in complex ui8 format from [%s]\n",samplereader->filename);
			chunk_reader = source_file_chunk_reader;
			set_frequency = source_file_set_frequency;
			set_gain = source_file_set_gain;
			set_sample_rate = source_file_set_sample_rate;
			break;
		case SOURCE_RTL:
			sdr_reader = new_rtlsdr_from_args(argc, argv, DEFAULT_BUF_LENGTH, rtl_sdr_callback);
			if (sdr_reader==NULL) exit(1);
			cf32buf = malloc( (DEFAULT_BUF_LENGTH/2*sizeof(uint8_t)) * sizeof(CF32_t));
			chunk_reader = rtl_sdr_chunk_reader;
			set_frequency = rtl_sdr_source_set_frequency;
			set_gain = rtl_sdr_source_set_gain;
			set_sample_rate = rtl_sdr_source_set_sample_rate;

			break;
		case SOURCE_TCP:
			tcp_reader = new_rtltcp_from_args(argc, argv, DEFAULT_BUF_LENGTH, rtl_tcp_callback);
			if (tcp_reader==NULL) exit(1);
			cf32buf = malloc( (DEFAULT_BUF_LENGTH/2*sizeof(uint8_t)) * sizeof(CF32_t));
			chunk_reader = rtl_tcp_chunk_reader;
			set_frequency = rtl_tcp_source_set_frequency;
			set_gain = rtl_tcp_source_set_gain;
			set_sample_rate = rtl_tcp_source_set_sample_rate;
			break;
		default:
			usage();
			exit(1);
	}

	sigact.sa_handler = sighandler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGTERM, &sigact, NULL);
	sigaction(SIGQUIT, &sigact, NULL);
	sigaction(SIGPIPE, &sigact, NULL);

	fprintf (stderr, "frequency shift from standard: %d\n", base_shift);
	// -15 = 0.03
	fprintf(stderr, "fm demod sqlevel %f\n", sqlevel);
	sqlevel = powf(10, sqlevel / 10);

	freq = davis_frq[davis_hop[davis_hop_index]] + base_shift + frq_offs ;
	set_sample_rate(samp_rate);
	set_frequency(freq);

	oscillator = new_cf32_tosc(0, get_phase_inc(40000, samp_rate));
	fir = new_cf32_fir(coeff, sizeof(coeff) / sizeof(float));
	fm_demod_gain = 4. * ((float) samp_rate)/ (2 * M_PI * (float) FSK_DEVIATION);
	bias_sample_delay = 32 * samp_rate / BAUD_RATE;


	fprintf(stderr, "fm demod gain %f\n", fm_demod_gain);
	fprintf(stderr, "fm demod bias_sample_delay %d\n", bias_sample_delay);

	fm_demod = new_cf32_fm_demod(fm_demod_gain, sqlevel, bias_sample_delay, 100, 192);
	fsk_demod = new_cf32_fsk_demod(samp_rate / BAUD_RATE, fsk_callback);
	davis_demod = new_davis_demod(davis_callback);

	if (dump_on_file){
		fprintf (stderr, "dumping data on [%s]\n", dump_file);
		samplewriter = new_samplewriter(dump_file, 0, S_UI8);
		if (samplewriter==NULL){
			exit (1);
		}
	}

	fprintf(stderr, "Acquiring samples...\n");

	hopper_next_time= millis()+25000;
	while (!do_exit){
		if (chunk_reader()==-1){
			break;
		}
	}
	if (dump_on_file){
		free_samplewriter(samplewriter);
	}


}

