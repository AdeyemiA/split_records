#define _GNU_SOURCE
#include <errno.h>
#include <error.h>
#include "rsplitimpl.h"

/* this util will split the large file by record*/
/* while keeping the file size respectively small*/

const char *argp_program_version = "rsplit 1.0";
const char *argp_program_bug_address = "<aoa2124@columbia.edu>";

/* Program documentation. */
static char doc[] =
  "\nInspired by the inability of the split unix utility, which,\
 possibly, splits a record across multiple files.\n\n\
 DESCRIPTION\
 \vSIZE  is  an  integer and optional unit\
 (example: 10M is 10*1024*1024). The allowed scales are K, M, G\
 (powers of 1024) for KB, MB, GB. \
 It is possible to state an incorrect number of columns and for the \
 records to be split across files. The correct number of records should \
 be specified, and the correct delimiter. In anycase of error, \
 The records with inconsistent number of delimiters, one less of column counts, \
 will be printed out to standard output irrespective of whether the verbose \
 option is selected.";

/* A description of the arguments we accept. */
static char args_doc[] = "pathname-to-input delimiter-in-quotes num-of-columns";

/* The options we understand. */
static struct argp_option options[] = {
  {"verbose",  'v', 0,       0, "Produce verbose output" },
  {"quiet",    'q', 0,       0, "Don't produce any output" },
  {"silent",   's', 0,       OPTION_ALIAS },
  {"prefix",   'p', "PREFIX",  0,
   "Prefix to be applied to Output FILE with suffixes to indicate order of records\
   x is default" },
  {"bytes",	   'b', "SIZE",		0, "put SIZE bytes per output file. Suffix the number\
  with k, m, or g for scaling. Without scales default to megabytes. Default size\
   if option omitted is 20MB"},
  {"header",   'h', 0,		0, "Input file has a header on the first line (unimplemented). \
	If specified and header inclusion is not specified, the header is stripped off the output. \
	If specified, the value read overrides that passed as an argument to the option \
	--include-header[=header_line] "},
  {"include-header", 'i', "HEADER",		OPTION_ARG_OPTIONAL, "Include the header from the first line. \
  Default is excluded. (unimplemented)"},
  {0,0,0,0, "The following options should be grouped together:" },
  {"abort",    OPT_ABORT, 0, 0, "Abort before showing any output"},
  { 0 }
};

int main(int argc, char *argv[])
{
	/* initialise the variables*/
	//printf("Here");
	char *prefix = "x";
	struct arguments arguments;

	/* Default values. */
	arguments.silent = 0;
	arguments.verbose = 0;
	arguments.output_file = prefix;
	arguments.abort = 0;
	arguments.has_header = 0;
	arguments.include_header = 0;
	arguments.header_line = NULL;
	char def_size[] = DEF_FILE_SIZE;
	arguments.size = def_size;
	//printf("There");

  	/* Parse our arguments; every option seen by parse_opt will be\
    reflected in arguments. */
    /* Our argp parser. */
	static struct argp argp = { options, parse_opt, args_doc, doc };
	argp_parse (&argp, argc, argv, 0, 0, &arguments);
	char *in_file = arguments.arg[0];
	const char *delim = arguments.arg[1];
	char *num_columns_str = arguments.arg[2];
	char *chunk_size_str = arguments.size;

	if (arguments.abort)
    	error (10, 0, "ABORTED");

	/* check if file is valid*/
    if (strlen(in_file) < 1) {
		fprintf(stderr, "%s \n", "Delimiter must be at least a character length");
		return -1;
	}

		/* get file size in MB and determine the total number of files needed */
	ulong f_size = (ulong) file_size(in_file);
	if (f_size == -1) {
		fprintf(stderr, "%s %s %s \n", strerror(errno), " occured while getting file properties for",  in_file);
		return -1;
	}

	/* check if prefix is valid, exit if not*/
	if (strlen(arguments.output_file) < 1) {
		fprintf(stderr, "%s \n", "The prefix must be at least a character length");
		return -1;
	}
	prefix = arguments.output_file;

	/* check if delim is at least strlen of 1*/
	if (strlen(delim) < 1) {
		fprintf(stderr, "%s \n", "Delimiter must be at least a character length");
		return -1;
	}

	char *size_chk = "1234567890.kKmMgG";
	char *num_chk = "1234567890";

	/* check if the number of columns in file is valid */
	if (strlen(num_columns_str) < 1) {
		fprintf(stderr, "%s \n ", "The file size must be specified and a valid size.");
		return -1;
	}

	if (!is_subset(num_chk, num_columns_str)) {
		fprintf(stderr, "%s \n", "Invalid character for number of columns");
		return -1;
	}
	
	/* check if the chunk size of file is valid */
	if (!is_subset(size_chk, chunk_size_str)) {
		fprintf(stderr, "%s \n", "Invalid character in the output file size");
		return -1;
	}

	/* strip off last char if metric scale used */
	float chunk_size = 0.0;
	char c = *(chunk_size_str + (strlen(chunk_size_str) - 1));
	switch (c)
	{
		case 'k': case 'K':
			strip_last_char(chunk_size_str);
			chunk_size = atof(chunk_size_str);
			if (chunk_size == 0) {
				fprintf(stderr, "%s \n", "Output file size cannot be zero");
				return -1;
			}
			chunk_size /= (float) K_BYTE;
			break;
		case 'm': case 'M':
			strip_last_char(chunk_size_str);
			chunk_size = atof(chunk_size_str);
			if (chunk_size == 0.0) {
				fprintf(stderr, "%s \n", "Output file size cannot be zero");
				return -1;
			}
			break;
		case 'g': case 'G':
			strip_last_char(chunk_size_str);
			chunk_size = atof(chunk_size_str);
			if (chunk_size == 0.0) {
				fprintf(stderr, "%s \n", "Output file size cannot be zero");
				return -1;
			}
			chunk_size *= K_BYTE;
			break;
		default:
			if (!is_subset(num_chk, &c)) {
				fprintf(stderr, "%s \n", "Unrecognised character in output file size");
				return -1;
			}
			chunk_size = atof(chunk_size_str);
			break;
	}

	/* specify a defualt chunk_size to be an option*/
	const int total_delim = (int) atof(num_columns_str) - 1;
	if (arguments.verbose)
		fprintf(stdout, "%s %ld MBytes \n", "The input file is", (ulong) f_size);

	/* get the width for suffix appended to the output files*/
	int num_of_suffix = calc_name_scheme(chunk_size, f_size);
	if (arguments.verbose)
		fprintf(stdout, "%s %d characters wide \n", "The output files' suffix is", num_of_suffix);
	char suffix[num_of_suffix + 1];
	for (int i = 0; i < num_of_suffix; ++i)
		suffix[i] = 'a';
	suffix[num_of_suffix] = '\0';

	/* get an input stream for the file to be read */
	FILE *in_file_stream;
	if ((in_file_stream = fopen(in_file, "r")) == NULL ) {
		fprintf(stderr, "%s %s %s \n", strerror(errno), " error while opening ",  in_file);
		return -1;
	}

	/* parse for header from first row and store the const pointer if there's a header */
	size_t buf_size = BUFFSIZE;
	char header_arr[buf_size];
	memset(header_arr, '\0', buf_size);
	char *header = header_arr;
	if (arguments.header_line != NULL) {
		if (strlen(arguments.header_line) > buf_size) {
			fprintf(stderr, "%s \n", "Conisder reducing the character length of the header passed");
			return -1;
		}
		strcpy(header, arguments.header_line);
	}

	if (arguments.has_header) {
		printf("%s\n", "Read header from file : incorrect");
		if (getline(&header, &buf_size, in_file_stream) < 0) {
			fprintf(stderr, "%s %s %s \n", strerror(errno), "Unable to read ", in_file);
			return -1;
		}
	}


	/* free the header. NOTE: Remove this line if the header is to be added to file*/
	/* comment the line below if header is to be written to evry file */
	//free(header);

	/* create the record buffer and initialize */
	char *cnt = ".cnt";
	char *record = malloc(RECORDSIZE * sizeof *record);
	if (record == NULL) {
		fprintf(stderr, "%s\n", "Failed to allocate space");
		return -1;
	}
	memset(record, '\0', RECORDSIZE * sizeof *record);

	size_t file_name_sz = strlen(prefix) + 1;
	char *input_file = malloc((file_name_sz + strlen(cnt)) * sizeof *input_file);
	if (input_file == NULL) {
		fprintf(stderr, "%s\n", "Failed to allocate space for the control filename");
		return -1;
	}
	memset(input_file, '\0', (file_name_sz + strlen(cnt)) * sizeof *input_file);

	file_name_sz += strlen(suffix);
	char *out_file = malloc(file_name_sz * sizeof *out_file);
	if (out_file == NULL) {
		fprintf(stderr, "%s\n", "Failed to allocate space for the output filename");
		return -1;
	}
	memset(out_file, '\0', file_name_sz * sizeof *out_file);	

	char *out_file_cnt = malloc((file_name_sz + strlen(cnt)) * sizeof *out_file_cnt);
	if (out_file_cnt == NULL) {
		fprintf(stderr, "%s\n", "Failed to allocate space for the output control filename");
		return -1;
	}
	memset(out_file_cnt, '\0', (file_name_sz + strlen(cnt)) * sizeof *out_file_cnt);

	strcpy(out_file, prefix);
	strcat(out_file, suffix);
	strcpy(input_file, prefix);
	strcat(input_file, cnt);
	strcpy(out_file_cnt, out_file);
	strcat(out_file_cnt, cnt); 
	FILE *input_record_cnt;
	if ((input_record_cnt = fopen(input_file, "w+")) == NULL) {
		fprintf(stderr, "%s %s %s \n", strerror(errno), " when opening ", input_file);
		exit_program(out_file, out_file_cnt, input_file, input_record_cnt);
		return -1;
	}
	int delim_count = 0;
	int new_record = 1;
	char *line = malloc(buf_size * sizeof *line);
	if (line == NULL) {
		fprintf(stderr, "%s %s \n", strerror(errno), "Failed to allocate space for the line buffer");
		return -1;
	}
	wipe_record(line);
	int fd;
	//int l_num_of_suffix = 0;
	if ((fd = open_out_file(out_file)) < 0) {
		fprintf(stderr, "%s %s %s \n", strerror(errno), " while opening file ",  out_file);
		exit_program(out_file_cnt, record, out_file, in_file_stream);
		free(line);
		return -1;
	}

	/* initialise the new file with the header */
	/* if every file, including the first, should have a header */
	if (arguments.include_header && header != NULL) {
		if (write_record(fd, header) < 0) {
			printf("Error: %s while writing file %s\n", strerror(errno), out_file);
			exit_program(header, record, out_file, in_file_stream);
			free(line);
			exit(-1);
		}
	}
	if (arguments.verbose)
		fprintf(stdout, "%s %s", "Created new file ", out_file);
	size_t old_space = 0;
	size_t old_sz = 0;
	size_t append = 0;
	ulong num_of_record = 0;
	FILE *out_stream_cnt;
	ulong total_record = 0;

	/* parse the rest of file to obtain records*/
	while (getline(&line, &buf_size, in_file_stream) > 0) {
		if (!new_record)	{
			/* if null-bytes in record cannot accomodate line's length + 1 null-byte, expand*/
			old_sz = malloc_usable_size(record);
			old_space = malloc_usable_size(record) - strlen(record);
			append = strlen(line) + 1;
			if (old_space < append) {
				record = realloc(record, append + old_sz);
				memset((record + old_sz), '\0', append);
			}
			strcat(record, line);
		} else {
			/* guaranteed to be a new record to be written*/
			wipe_record(record);
			if (malloc_usable_size(record) < strlen(line)) {
				/* expand record*/
				old_sz = malloc_usable_size(record);
				append = strlen(line) + 1 - old_sz;
				record = (char *) realloc(record, old_sz + append);
				memset((record + old_sz), '\0', append);
			}
			strcpy(record, line);

			/* no longer a new record*/
			new_record = 0;

			/* create new file if previous file is large enough*/
			/* guaranteed to be a new record in a new file*/
			f_size = (ulong) file_size(out_file);
			if (f_size < 0) {
				fprintf(stderr, "%s %s %s \n", strerror(errno), " occured while getting properties for ", out_file);
				exit_program(out_file_cnt, record, out_file, in_file_stream);
				free(line);
				return -1;
			}
			if (f_size >= chunk_size) {
				/* write the number of records to file cnt*/
				if((out_stream_cnt = fopen(out_file_cnt, "w+")) == NULL) {
					fprintf(stderr, "%s %s %s \n", strerror(errno), " occured while opening ", out_file_cnt);
					exit_program(out_file_cnt, record, out_file, in_file_stream);
					free(line);
					return -1;
				}
				if (fprintf(out_stream_cnt, "%ld", num_of_record) < 0) {
					printf("Error: %s while writing file %s\n", strerror(errno), out_file_cnt);
					exit_program(out_file_cnt, record, out_file, in_file_stream);
					fclose(out_stream_cnt);
					free(line);
					exit(-1);
				}
				fclose(out_stream_cnt);
				if (arguments.verbose)
					fprintf(stdout, " : %ld MB\n", f_size);
				if (close(fd) < 0) {
					fprintf(stderr, "%s %s %s \n", strerror(errno), " occured while closing ", out_file);
					exit_program(out_file_cnt, record, out_file, in_file_stream);
					free(line);
					return -1;
				}

				/* initialise the record count per file, and create new file and store in out_file pointer*/
				total_record += num_of_record;
				num_of_record = 0;
				if (get_next_filename(out_file, num_of_suffix) < 0) {
					fprintf(stderr, "%s \n", "An error occured while getting the next output filename");
					exit_program(out_file_cnt, record, out_file, in_file_stream);
					free(line);
					return -1;
				}

				wipe_record(out_file_cnt);
				strcpy(out_file_cnt, out_file);
				strcat(out_file_cnt, cnt);

				/* open new file with new filename and write the out_file*/
				if ((fd = open_out_file(out_file)) < 0) {
					fprintf(stderr, "%s %s %s \n", strerror(errno), " occured while opening ", out_file);
					exit_program(out_file_cnt, record, out_file, in_file_stream);
					free(line);
					exit(-1);
				}

				/* if every file should have a header*/
				if (arguments.include_header  && header != NULL) {
					if (write_record(fd, header) < 0) {
						printf("Error: %s while writing file %s\n", strerror(errno), out_file);
						exit_program(header, record, out_file, in_file_stream);
						free(line);
						exit(-1);
					}
				}
				if (arguments.verbose)
					fprintf(stdout, "%s %s", "Created new file ", out_file);
			}
		}

		/* count if the delimiter count means a record is found */
		delim_count += count_delim(delim, line);
		if (delim_count >= total_delim) {
			if (delim_count > total_delim) {
				fprintf(stdout, " : %d delimiters found in \n", delim_count);
				printf("%s \n", record);
			}
			delim_count = 0;
			new_record = 1;
			if (write_record(fd, record) < 0) {
				fprintf(stderr, "%s %s %s \n", strerror(errno), " occured while writing ", out_file);
				exit_program(out_file_cnt, record, out_file, in_file_stream);
				free(line);
				exit(-1);
			}
			++num_of_record;
			wipe_record(line);
		}
	}
	if (arguments.verbose)
		fprintf(stdout, " : %ld MB \n", (ulong) file_size(out_file));

	/* Open the last control file and write the record counts to file*/
	if((out_stream_cnt = fopen(out_file_cnt, "w+")) == NULL) {
		fprintf(stderr, "%s %s %s \n", strerror(errno), " while opening ", out_file_cnt);
		exit_program(out_file_cnt, record, out_file, in_file_stream);
		free(line);
		return -1;
	}
	if (fprintf(out_stream_cnt, "%ld", num_of_record) < 0) {
		fprintf(stderr, "%s %s %s \n", strerror(errno), " while writing ", out_file_cnt);
		exit_program(out_file_cnt, record, out_file, in_file_stream);
		fclose(out_stream_cnt);
		free(line);
		exit(-1);
	}

	/* close files and stream*/
	exit_program(out_file_cnt, record, out_file, in_file_stream);
	free(line);
	if (arguments.has_header)
		free(header);
	total_record += num_of_record;

	/* write the total record counts to file*/
	if (fprintf(input_record_cnt, "%ld", total_record) < 0) {
		fprintf(stderr, "%s %s %s \n", strerror(errno), " while writing ", input_file);
		fclose(input_record_cnt);
		exit(-1);
	}
	fclose(input_record_cnt);
	free(input_file);
	close(fd);
	fprintf(stdout, "%s \n", "Record split completed \n");
	return 0;
}