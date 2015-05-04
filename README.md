# record_split
Inspired by the flaw in the split unix utility for splitting exported files with table records.  This tool does not split record across multiple files like split does. It splits the file, preserving records in a file and not across files.


Written in C. Fork the repository and compile by typing `make all` on the command line


i.e. $make all


Run Usage: ./rsplit path/to/file delimiter_in_double_quotes num_of_columns output_file_size_MB output_file_prefix(optional, default is 'x')
