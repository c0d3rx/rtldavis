#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include "ztypes.h"
#include "sample_reader.h"

extern void usage(void);
void samplereader_usage(void) {
	usage();
	fprintf(stderr, "\t[--file-format=S_CF32|...]\n"
			"\t--file-in=<input file>\n\n");
	exit(1);
}

t_SAMPLEREADER* new_samplereader_from_args(int argc, char **argv,
		int sample_format_out, unsigned int chunk_size,
		void (*callback)(t_SAMPLEREADER*, void*, unsigned int)) {

	static struct option long_options[] = {
			{ "file-in", optional_argument,	NULL, 0 },
			{ "file-format", optional_argument, NULL, 0 },
			{ NULL, 0,	NULL, 0 } };
	int c;
	int option_index = 0;
	char *filename=NULL;
	int sample_format_in = S_CUI8;

	optind = 0;

	while ((c = getopt_long(argc, argv, ":h", long_options, &option_index)) != -1) {
		switch (c) {
		case 0:
			switch (option_index) {
			case 0:
				filename = malloc(strlen(optarg)+1);
				strcpy(filename, optarg);
				break;
			}
			break;
		case 'h':
			samplereader_usage();
			break;
		}
	}

	return new_samplereader(filename, sample_format_in, sample_format_out, chunk_size, callback);
}

t_SAMPLEREADER* new_samplereader(char *filename, int sample_format_in,
		int sample_format_out, unsigned int chunk_size,
		void (*callback)(t_SAMPLEREADER*, void*, unsigned int)) {
	t_SAMPLEREADER *rt;

	rt = malloc(sizeof(t_SAMPLEREADER));
	rt->filename = malloc(strlen(filename) + 1);
	strcpy(rt->filename, filename);

	rt->handle = fopen(filename, "rb");
	if (rt->handle == NULL){
		fprintf(stderr, "Error opening [%s], errno [%d] (%s)\n",rt->filename, errno, strerror(errno));
		return NULL;
	}

	rt->chunk_size = chunk_size;
	rt->sample_format_in = sample_format_in;
	rt->sample_format_out = sample_format_out;
	rt->sample_size_in = ztype_get_size(sample_format_in);
	rt->sample_size_out = ztype_get_size(sample_format_out);

	rt->buffer_index = 0;
	rt->sample_buffer_in = malloc(rt->sample_size_in * chunk_size);
	rt->sample_buffer_out = malloc(rt->sample_size_out * chunk_size);

// functions
	rt->read_chunk = read_samplereader_chunk;
	// rt->read = read_samplereader;

	rt->callback = callback;
	return rt;
}

int read_samplereader_chunk(t_SAMPLEREADER *r) {

	size_t samples_to_read;
	unsigned int i;

	size_t rt = 0;
	size_t fr;

	samples_to_read = r->chunk_size;

	fr = fread(r->sample_buffer_in, r->sample_size_in, samples_to_read,
			r->handle);

	// convert the sample and call callback function
	if ((r->sample_format_in == S_CUI8) && (r->sample_format_out == S_CI16)) {
		CUI8_t *in = (CUI8_t*) r->sample_buffer_in;
		CI16_t *out = (CI16_t*) r->sample_buffer_out;
		for (i = 0; i < fr; i++) {
			*out++ = uiq8_iq16(*in++);
		}
	}
	if ((r->sample_format_in == S_CUI8) && (r->sample_format_out == S_CF32)) {
		CUI8_t *in = (CUI8_t*) r->sample_buffer_in;
		CF32_t *out = (CF32_t*) r->sample_buffer_out;
		for (i = 0; i < fr; i++) {
			*out++ = uiq8_fc32(*in++);
		}
	}

	r->callback(r, r->sample_buffer_out, fr);

	return fr == samples_to_read ? 0 : -1;

}

void free_samplereader(t_SAMPLEREADER *r) {

	if (r->handle) {
		fclose(r->handle);
	}
	free(r->sample_buffer_in);
	free(r->sample_buffer_out);
	free(r);
}

