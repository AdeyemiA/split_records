# Split Records

Inspired by the deficiency in the split unix utility, which, possibly, splits a
record across multiple files.

Written in C. Fork the repository and compile by typing `make` on the command line in the root directory of the project.

```sh
/home/split_records$ make
```
### Usage

###### ./rsplit  [OPTION...]  pathname-to-input delimiter-in-quotes num-of-columns
> ./rsplit [-hqsv?V] [-b SIZE] [-i[HEADER]] [-p PREFIX] [--bytes=SIZE]
            [--header] [--include-header[=HEADER]] [--prefix=PREFIX] [--quiet]
            [--silent] [--verbose] [--abort] [--help] [--usage] [--version]
            pathname-to-input delimiter-in-quotes num-of-columns


####DESCRIPTION 

* -b, --bytes=SIZE          
>*  put SIZE bytes per output file. Suffix the number                           with k, m, or g for scaling. Without scales
>*      default to megabytes. Default size if the option                           omitted is 20MB

*	-c, --count
>*	Produce only the total number of records in a .cnt file.
>*	The input file is not split.

*   -h, --header               
>*      Input file has a header on the first line. If specified and header
>*        inclusion is not specified, the header is stripped off the output. 	If specified, the value read
>*        overrides that passed as an argument to the option        --include-header[=header_line]
>*		  Default is input file has no header.

*   -i, --include-header[=HEADER]   
>*  Include the header from the first line.          Default is excluded. If this option is specified
>*  Without the optional header arg or --header                             included option, no header is written to the
>*  output files. The number of records excludes the                             header. 

*  -p, --prefix=PREFIX        
>*  Prefix to be applied to Output FILE with suffixes                             to indicate order of records.   The default is 'x'

*  -q, -s, --quiet, --silent  
>*  Don't produce any output

*  -v, --verbose              
>*  Produce verbose output

###### The following options should be grouped together:
*   --abort                
>*   Abort before showing any output

*   -?, --help 
>*      Give this help list

*   --usage
>*      Give a short usage message

*   -V, --version              
>*  Print program version

### Version
1.0.0

License
----

Apache v2.0
