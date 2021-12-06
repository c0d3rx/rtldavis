#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "ztypes.h"
#include "sample_writer.h"

t_SAMPLEWRITER* new_samplewriter(char *filename, bool append, int sample_format) {
	t_SAMPLEWRITER *rt;

	rt = malloc(sizeof(t_SAMPLEWRITER));
	rt->filename = malloc(strlen(filename) + 1);
	strcpy(rt->filename, filename);

	rt->handle = fopen(filename, "wb");
	rt->sample_format = sample_format;
	rt->sample_size = ztype_get_size(sample_format);
	rt->buffer_index = 0;
	rt->sample_buffer = malloc(rt->sample_size * SAMPLEWRITER_BUFFERSIZE);

// functions
//  rt->read = (size_t (*)(void *, unsigned int)) read_samplereader;

	rt->write =
			(size_t (*)(void*, void*, int, unsigned int)) write_samplewriter;

	printf("filename %s\n", rt->filename);
	return rt;
}

/**

 */
size_t write_samplewriter(t_SAMPLEWRITER *w, void *sample, int format,
		unsigned int nr_samples) {

	unsigned int id, is;
	size_t ws = 0;
	int size_of_type = ztype_get_size(format);

	//    int16_t i16;
	//  float flt;

	id = w->buffer_index;	// destination index
	is = 0;					// source index
	while (nr_samples--) {


		if (w->sample_format == format) {
			memcpy(w->sample_buffer+id*size_of_type, sample+is*size_of_type, size_of_type);
			id++;
			is++;
		}

		if (id == SAMPLEWRITER_BUFFERSIZE) {
			w->buffer_index = id;
			ws = flush_samplewriter(w);
			id = 0;
			w->buffer_index = 0;
		}
	}
	w->buffer_index = id;

	return ws;

}

size_t flush_samplewriter(t_SAMPLEWRITER *w) {
	size_t ws = 0;

	if (w->buffer_index) {
		ws = fwrite(w->sample_buffer, w->sample_size, w->buffer_index,
				w->handle);
		w->buffer_index = 0;
	}
	return ws;

}

void free_samplewriter(t_SAMPLEWRITER *w) {
	flush_samplewriter(w);
	if (w->handle) {
		fclose(w->handle);
	}
	free(w);
}

