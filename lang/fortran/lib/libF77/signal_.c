#include "f2c.h"

typedef int (*sig_type)();
extern sig_type signal();

integer signal_(sigp, procp)
integer *sigp, *procp;
{
	int sig;
	sig_type proc;
	sig = *sigp;
	proc = *(sig_type *)procp;

	return (integer)signal(sig, proc);
	}
