/* $Header$ */
#include <stdio.h>

struct _io_buf _stdin = {
	0, 0, IO_READMODE , 0, 0, 0
};

struct _io_buf _stdout = {
	1, 0, IO_WRITEMODE, 0, 0, 0
};

struct _io_buf _stderr = {
	2, 0, IO_WRITEMODE + IO_UNBUFF, 0, 0, 0
};

struct  _io_buf  *_io_table[_NFILES] = {
	&_stdin,
	&_stdout,
	&_stderr,
	0
};
