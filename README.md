# record_split
Inspired by the flaw in the split unix utility for splitting exported files with table records.  This tool does not split record across multiple files like split does. It splits the file, preserving records in a file and not across files.


Written in C. Fork the repository and compile by typing `make all` on the command line


i.e. $make all


Run Usage: Usage: rsplit [-hqsv?V] [-b SIZE] [-i[HEADER]] [-p PREFIX] [--bytes=SIZE]
            [--header] [--include-header[=HEADER]] [--prefix=PREFIX] [--quiet]
            [--silent] [--verbose] [--abort] [--help] [--usage] [--version]
            pathname-to-input delimiter-in-quotes num-of-columns
