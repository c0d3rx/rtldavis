#ifndef _SAMPLEREADER_H_
 #define _SAMPLEREADER_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>



typedef struct SAMPLEREADER {
  char *filename;
  bool append;
  FILE *handle;
  int sample_format_in;
  int sample_format_out;

  unsigned int chunk_size;

  void *sample_buffer_in;
  unsigned int sample_size_in;
  
  void *sample_buffer_out;
  unsigned int sample_size_out;

  unsigned int buffer_index;

  void (*callback)(struct SAMPLEREADER *, void * sample, unsigned int samples);
  int (*read_chunk)(struct SAMPLEREADER*);
  
} t_SAMPLEREADER;


t_SAMPLEREADER *new_samplereader_from_args (int, char **, int, unsigned int, void (*)(t_SAMPLEREADER*, void *, unsigned int));
t_SAMPLEREADER *new_samplereader(char *, int, int, unsigned int, void (*)(t_SAMPLEREADER *, void *, unsigned int) );
int read_samplereader_chunk(t_SAMPLEREADER * r);
void free_samplereader(t_SAMPLEREADER * r);


#endif
