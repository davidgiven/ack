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
	{ -1,		0}
};

extern char		*_hol0();
extern char		*_argv[];
extern			exit();

_catch(trapno)
	int trapno;
{
	register struct errm *ep = &errors[0];
	char *errmessage;
	char		*pp[8];
	register char **qq = &pp[0];
	register char *p;
	char *q;
	int i;

	if (p = FILN)
		*qq++ = p;
	else
		*qq++ = _argv[0];
	p = &("xxxxxxxxxxx: "[11]);
	if (i = LINO) {
		if (i < 0) {
			/* ??? */
			*qq++ = ", -";
			i = -i;
		}
		else
			*qq++ = ", ";
		do
			*--p = i % 10 + '0';
		while (i /= 10);
	}
	*qq++ = p;
	while (ep->errno != trapno && ep->errmes != 0) ep++;
	if (ep->errmes)
		*qq++ = ep->errmes;
	else {
		*qq++ = "error number";
		p = &("xxxxxxxxxxx: "[11]);
		i = trapno;
		if (i < 0) {
			/* ??? */
			*qq++ = "-";
			i = -i;
		}
		do
			*--p = i % 10 + '0';
		while (i /= 10);
		*qq++ = p;
	}
	*qq++ = "\n";
	*qq = 0;
	qq = pp;
	while (q = *qq++) {
		p = q;
		while (*p)
			p++;
		if (write(2,q,p-q) < 0)
			;
	}
	exit(trapno);
}
