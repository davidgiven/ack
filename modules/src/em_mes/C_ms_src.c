#include <em.h>
#include <em_mes.h>

C_ms_src(nlines, filnam)
	int nlines;
	char *filnam;
{
	C_mes_begin(ms_src);
	C_cst((arith)nlines);
	C_scon(filnam, strlen(filnam) + 1);
	C_mes_end();
}
