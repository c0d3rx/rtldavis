#ifndef _SAMPLEWRITER_H_
 #define _SAMPLEWRITER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define SAMPLEWRITER_BUFFERSIZE 512


typedef struct SAMPLEWRITER {
  char *filename;
  bool append;
  FILE *handle;
  int sample_format;
  unsigned int sample_size;
  
  void *sample_buffer;
  unsigned int buffer_index;

  size_t (*write)(void * w, void * sample, int sample_format, unsigned int nr_samples);
  
  
} t_SAMPLEWRITER;


t_SAMPLEWRITER *new_samplewriter(char *filename, bool append, int sample_format);
size_t write_samplewriter(t_SAMPLEWRITER * w, void * sample, int sample_format, unsigned int nr_samples);
size_t flush_samplewriter(t_SAMPLEWRITER * w);
void free_samplewriter(t_SAMPLEWRITER * w);


#endif
