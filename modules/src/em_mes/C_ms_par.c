#include <em.h>
#include <em_mes.h>

C_ms_par(nparams)
	arith nparams;
{
	C_mes_begin(ms_par);
	C_cst(nparams);
	C_mes_end();
}
