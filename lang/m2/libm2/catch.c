#include <em_abs.h>

static struct errm {
	int errno;
	char *errmes;
} errors[] = {
	{ EARRAY,	"array bound error"},
	{ ERANGE,	"range bound error"},
	{ ESET,		"set bound error"},
	{ EIOVFL,	"integer overflow"},
	{ EFOVFL,	"floating overflow"},
	{ EFUNFL,	"floating underflow"},
	{ EIDIVZ,	"divide by 0"},
	{ EFDIVZ,	"divide by 0.0"},
	{ EIUND,	"undefined integer"},
	{ EFUND,	"undefined float"},
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

	{ 64,		"stack size of process too large"},
	{ 65,		"too many nested traps + handlers"},
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
		*s = 0;
		s = buf;
		while (*p++ = *s++) /* nothing */;
		Traps_Message(q, 0, (int) (p - q), 1);
	}
	exit(trapno);
}
