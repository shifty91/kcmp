# KCMP #

## About ##

`kcmp` is small tool for comparing two files byte by byte. If the files differ 1
is returned, otherwise 0 or 2 if an error ocurred.

## Usage ##

    usage: kcmp [-v] <file0> <file1>
	  options:
	    --verbose, -v: enable verbose output

## Build ##

The build process uses a simple Makefile which uses GNU features. Thus, you have
to use GNU Make for building it.

On Linux use:

    $ make

On UNIX systems use:

    $ gmake

By default the Makefile uses clang as compiler, but you can override some
variables by hand. Example for gcc:

    $ CC=gcc6 gmake

## License ##

BSD 2-clause

## Author

(C) 2015 Kurt Kanzenbach <kurt@kmk-computers.de>
