#include "header.h"

open_back( filename)
char *filename;
{
	if ( filename == (char *) 0) {
		codefile= STDOUT;
#ifdef __solaris__
		fprint(codefile, ".section \".text\"\n");
#endif
		return 1;
	}
#ifndef sys_close
	if ( sys_open( filename, OP_WRITE, &codefile)) {
#else
	if ((codefile = fopen(filename, "w")) != NULL) {
#endif
#ifdef __solaris__
		fprint(codefile, ".section \".text\"\n");
#endif
		return 1;
	}
	return 0;
}
