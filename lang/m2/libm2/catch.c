#include <em_abs.h>
#include <m2_traps.h>

static struct errm {
	int errno;
	char *errmes;
} errors[] = {
	{ EARRAY,	"array bound error"},
	{ ERANGE,	"range bound error"},
	{ ESET,		"set bound error"},
	{ EIOVFL,	"integer overflow"},
	{ EFOVFL,	"real overflow"},
	{ EFUNFL,	"real underflow"},
	{ EIDIVZ,	"divide by 0"},
	{ EFDIVZ,	"divide by 0.0"},
	{ EIUND,	"undefined integer"},
	{ EFUND,	"undefined real"},
	{ ECONV,	"conversion error"},

	{ ESTACK,	"stack overflow"},
	{ EHEAP,	"heap overflow"},
	{ EILLINS,	"illegal instruction"},
	{ EODDZ,	"illegal size argument"},
	{ ECASE,	"case error"},
	{ EMEMFLT,	"addressing non existent memory"},
	{ EBADPTR,	"bad pointer used"},
	{ EBADPC,	"program counter out of range"},
	{ EBADLAE,	"bad argument of lae"},
	{ EBADMON,	"bad monitor call"},
	{ EBADLIN,	"argument if LIN too high"},
	{ EBADGTO,	"GTO descriptor error"},

	{ M2_TOOLARGE,	"stack size of process too large"},
	{ M2_TOOMANY,	"too many nested traps + handlers"},
	{ M2_NORESULT,	"no RETURN from procedure function"},
	{ M2_UOVFL,	"cardinal overflow"},
	{ M2_FORCH,	"Warning: FOR-loop control variable was changed in the body"},
	{ M2_ENDPROCS,	"too many procedures to be called on program termination"},
	{ -1,		0}
};

extern			exit();

_catch(trapno)
	int trapno;
{
	register struct errm *ep = &errors[0];
	char *errmessage;
	char buf[20];
	register char *p, *s;
	char *q;

	while (ep->errno != trapno && ep->errmes != 0) ep++;
	if (p = ep->errmes) {
		while (*p) p++;
		Traps_Message(ep->errmes, 0, (int) (p - ep->errmes), 1);
	}
	else {
		int i = trapno;

		q = "error number xxxxxxxxxxxxx";
		p = &q[13];
		s = buf;
		if (i < 0) {
			i = -i;
			*p++ = '-';
		}
		do
			*s++ = i % 10 + '0';
		while (i /= 10);
		while (s > buf) *p++ = *--s;
		*p = 0;
		Traps_Message(q, 0, (int) (p - q), 1);
	}
	if (trapno != M2_FORCH) exit(trapno);
	SIG(_catch);
}
