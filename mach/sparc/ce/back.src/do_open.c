#include "header.h"

open_back( filename)
char *filename;
{
	if ( filename == (char *) 0) {
		codefile= stdout;
#ifdef __solaris__
		fprintf(codefile, ".section \".text\"\n");
#endif
		return 1;
	}
#ifndef sys_close
	if ( sys_open( filename, OP_WRITE, &codefile)) {
#else
	if ((codefile = fopen(filename, "wb")) != NULL) {
#endif
#ifdef __solaris__
		fprintf(codefile, ".section \".text\"\n");
#endif
		return 1;
	}
	return 0;
}
