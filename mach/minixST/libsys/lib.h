#include <minix/const.h>
#include <minix/type.h>
#include <minix/callnr.h>
#include <errno.h>

extern message M;

#define MM                 0
#define FS                 1

extern int callm1(), callm3(), callx(), len();
extern int errno;
extern int begsig();		/* interrupts all vector here */
