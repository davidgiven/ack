#include <stdio.h>

unsigned char  _sobuf[BUFSIZ];
unsigned char  _sibuf[BUFSIZ];

struct _io_buf _stdin = {
	0, 0, IO_READMODE , _sibuf, _sibuf
};

struct _io_buf _stdout = {
	1, 0, IO_WRITEMODE, 0, 0
};

struct _io_buf _stderr = {
	2, 0, IO_WRITEMODE + IO_UNBUFF, NULL, NULL
};

struct  _io_buf  *_io_table[_NFILES] = {
	&_stdin,
	&_stdout,
	&_stderr,
	0
};
