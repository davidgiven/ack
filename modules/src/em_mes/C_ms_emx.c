#include <em.h>
#include <em_mes.h>

C_ms_emx(wsiz, psiz)
	arith wsiz, psiz;
{
	C_mes_begin(ms_emx);
	C_cst(wsiz);
	C_cst(psiz);
	C_mes_end();
}
