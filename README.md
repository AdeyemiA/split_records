# record_split
Inspired by the deficiency in the split unix utility in splitting exported files with table records.  This tool does not split record across multiple files like split does. It splits the file, preserving records in a file and not across files.


Written in C. Fork the repository and compile by typing `make` on the command line in the root directory of the project.


i.e. /home/split_records$make


Run Usage: Usage: ./rsplit [-hqsv?V] [-b SIZE] [-i[HEADER]] [-p PREFIX] [--bytes=SIZE]
            [--header] [--include-header[=HEADER]] [--prefix=PREFIX] [--quiet]
            [--silent] [--verbose] [--abort] [--help] [--usage] [--version]
            pathname-to-input delimiter-in-quotes num-of-columns

*Usage:* rsplit [OPTION...] pathname-to-input delimiter-in-quotes num-of-columns

Inspired by the deficiency in the split unix utility, which, possibly, splits a
record across multiple files.

 *DESCRIPTION* 

  ```-b, --bytes=SIZE           put SIZE bytes per output file. Suffix the number
                             with k, m, or g for scaling. Without scales
                             default to megabytes. Default size   if option
                             omitted is 20MB
  ```-h, --header               Input file has a header on the first line
                             (unimplemented). 	If specified and header
                             inclusion is not specified, the header is stripped
                             off the output. 	If specified, the value read
                             overrides that passed as an argument to the option
                             	--include-header[=header_line] 
  -i, --include-header[=HEADER]   Include the header from the first line.
                             Default is excluded. If this option is specified
                             Without the optional header arg or --header
                             included option, no header is written to the
                             output files. The number of records excludes the
                             header. 
  -p, --prefix=PREFIX        Prefix to be applied to Output FILE with suffixes
                             to indicate order of records   x is default
  -q, -s, --quiet, --silent  Don't produce any output
  -v, --verbose              Produce verbose output

 The following options should be grouped together:
      --abort                Abort before showing any output

  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

SIZE  is  an  integer and optional unit (example: 10M is 10*1024*1024). The
allowed scales are K, M, G (powers of 1024) for KB, MB, GB.  It is possible to
state an incorrect number of columns and for the  records to be split across
files. The correct number of records should  be specified, and the correct
delimiter. In anycase of error,  The records with inconsistent number of
delimiters, one less of column counts,  will be printed out to standard output
irrespective of whether the verbose  option is selected.

Report bugs to <aoa2124@columbia.edu>
