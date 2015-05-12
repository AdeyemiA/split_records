#ifndef _RSPLIT_H_
#define _RSPLIT_H_
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <argp.h>

#define NUM_ARGS 3
#define PRG_NAME "rsplit"
#define BUFFSIZE 10000
#define DEF_FILE_SIZE "20m"
#define RECORDSIZE 20000
#define NUM_OF_ALPHA 26
#define K_BYTE 1024

/* Keys for options without short-options. */
#define OPT_ABORT  1            /* –abort */

typedef unsigned long int ulong;

/* Used by main to communicate with parse_opt. */
struct arguments
{
	  /* arg[0] - filepath, arg[1] - delimiter, arg[2] - number of columns */
  char *arg[NUM_ARGS];                   
  int silent, verbose, abort, use_prefix, inputfile_has_header, count_record, include_header;   /* ‘-s’, ‘-v’, ‘--abort’ */
  char *output_file, *header_string;            /* file arg to ‘--output’ */
  char *size;             /* output_file size */
};

error_t parse_opt (int key, char *arg, struct argp_state *state);
int calc_name_scheme(const int small_size, const off_t large_size);
off_t file_size(const char *pathname);
int count_delim(const char *delim, const char *line);
void exit_program(char *, char *, char *, FILE *);
int open_out_file(const char *filename);
int write_record(int fd, const char *line);
void wipe_buffer(char *);
int get_next_filename(char *filename, int num_of_suffix);
int is_subset(const char *super, const char *sub);
void strip_last_char(char *str);
#endif