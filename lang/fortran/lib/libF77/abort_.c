#include "stdio.h"
#include "f2c.h"

extern VOID sig_die();

VOID abort_()
{
sig_die("Fortran abort routine called", 1);
}
