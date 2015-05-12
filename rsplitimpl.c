#include "rsplitimpl.h"

/* Parse a single option. */
error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
  struct arguments *arguments = state->input;

  switch (key)
    {
    case 'q': case 's':
    	arguments->silent = 1;
      arguments->verbose = 0;
      break;
    case 'v':
      arguments->verbose = 1;
      break;
    case 'p':
    	arguments->use_prefix = 1;
      arguments->output_file = arg;
      break;
    case 'b':
    	arguments->size = arg;
      break;
    case 'h':
    	arguments->inputfile_has_header = 1;
    	break;
    case 'c':
    	arguments->count_record = 1;
    	break;
   	case 'i':
   		if (arg != NULL)
   			arguments->header_string = arg;
   		arguments->include_header = 1;
   		break;
    case OPT_ABORT:
      arguments->abort = 1;
      break;

    case ARGP_KEY_NO_ARGS:
      argp_usage (state);
      break;
    case ARGP_KEY_ARG:
      /* Here we check if the arg index has exceeded and output usage info */
      if (state->arg_num >= NUM_ARGS) {
      		fprintf(stderr, "%s \n", "Too many arguments");
         	argp_usage(state);
      }

      arguments->arg[state->arg_num] = arg;
      break;
    case ARGP_KEY_END:
    	/* check if at the end, the count indicates the total args*/
    	if (state->arg_num < NUM_ARGS) {
    		fprintf(stderr, "%s \n", "Too little arguments");
    		argp_usage(state);
    	}
    	break;
    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

int calc_name_scheme(const int small_size, const off_t large_size) 
{
	/* n = roundup(log(file_size_MB/size_per_file_MB) / log(NUM_OF_ALPHA))*/
	/* n is the number of elements in the array*/
	return (int) ceil(log(large_size/(double) small_size) / log((double) NUM_OF_ALPHA));
}

off_t file_size(const char *filename)
{
	struct stat buf;
	if (stat(filename, &buf) < 0)
		return -1;

	return ceil(buf.st_size/(double)(K_BYTE * K_BYTE));
}

int count_delim(const char *delim, const char *line)
{
	size_t num_char = strlen(delim);
	//char delim_chars[num_char];
	int delim_count = 0;
	int num_char_found = 0;
	int c = 0;
	int num_read = 0;
	/* check the char buffer pointed by line for the delimiters*/
	while ((c = *(line + num_read)) != '\0') {
		++num_read;
		if (c == *(delim + num_char_found)) {
			++num_char_found;

			/* if there are other delim char to find*/
			if (num_char_found < num_char) {
				while ((num_char_found < num_char) && (c = *(line + num_read)) != '\0') {
					++num_read;
					if (c == *(delim + num_char_found))
						++num_char_found;
					else
						break;
				}
			}

			/* check if all is found, increment delim_count if so*/
			if (num_char_found == num_char)
				++delim_count;
		}
		num_char_found = 0;
	}
	return delim_count;
}

void exit_program(char *out_file_cnt, char *record, char *out_file, FILE *in_file_stream) 
{
	//va_list varg;
	//va_start(varg);
	
	free(out_file_cnt);
	free(record);
	free(out_file);
	fclose(in_file_stream);	
}

int open_out_file(const char *filename) 
{
	int fd;
	if ((fd = open(filename, O_CREAT | O_RDWR , S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) < 0) {
		fprintf(stderr, "%s %s %s \n", strerror(errno), " while opening ", filename);
		return -1;
	}
	return fd;
}

int write_record(int fd, const char *line) 
{
	ssize_t num_byte;
	if ((num_byte = write(fd, line, strlen(line) * sizeof *line)) < 0) {
		fprintf(stderr, "%s %s %s \n", strerror(errno), " while writing to ", line);
		return -1;			
	}
	return num_byte;
}

void wipe_buffer(char *buf)
{
	memset(buf, '\0', malloc_usable_size(buf));
}

int get_next_filename(char *filename, int num_of_suffix)
{
	int l_num_of_suffix = 0;
	++l_num_of_suffix;
	for (; *(filename + strlen(filename) - l_num_of_suffix) == 'z' && (l_num_of_suffix <= num_of_suffix); ++l_num_of_suffix) {
		/* check if -1th is the max and report error is so */
		if (l_num_of_suffix == num_of_suffix) {
			fprintf(stderr, "%s \n", "While splitting file, an unexpected behaviour occured");
			return -1;
		}

		/* change -1th position to 'a' */
		*(filename + strlen(filename) - l_num_of_suffix) = 'a';
	}

	/* increase the char by 1 which is positioned l_num_of_suffix away from the end guaranteed not to be 'z'*/
	*(filename + strlen(filename) - l_num_of_suffix) += 1;
	return 0;
}

int is_subset(const char *super, const char *sub)
{
	int found = 1;
	int i, j = 0, c = 0;
	for (; *(sub + j) != '\0' && found; ++j) {
		found = 0;
		c = *(sub + j);
		for (i = 0; i < strlen(super); ++i) {
			if (c == *(super + i)) {
				found = 1;
				break;
			}
		}
	}
	return found;
}

void strip_last_char(char *str)
{
	*(str + (strlen(str) - 1)) = '\0';
}
