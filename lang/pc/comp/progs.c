/* TYDELYK !!!!!! */

#include	"debug.h"

#include	<assert.h>
#include	<em.h>

#include	"LLlex.h"
#include	"def.h"
#include	"main.h"
#include	"scope.h"
#include	"type.h"

arith cnt = 2;			/* standaard input & output */
int inpflag = 0;		/* std input gedefinieerd of niet */
int outpflag = 0;		/* std output gedefinieerd of niet */
label con_label;

set_inp()
{
	inpflag = 1;
}

set_outp()
{
	outpflag = 1;
}

set_prog(df)
	struct def *df;
{
	cnt++;
	df->df_flags |= 0x40;
}

make_con()
{
	register struct def *df;

	con_label = ++data_label;
	C_df_dlb(con_label);
	C_con_cst(cnt);

	if( inpflag )
		C_con_dnam("input", (arith) 0);
	else
		C_con_cst((arith) -1);

	if( outpflag )
		C_con_dnam("output", (arith) 0);
	else
		C_con_cst((arith) -1);

	for( df = GlobalScope->sc_def; df; df = df->df_nextinscope )
		if( df->df_flags & 0x40 )	{
			C_con_dnam(df->var_name, (arith) 0);
			cnt--;
		}

	assert(cnt == 2);
}

call_ini()
{
	C_lxl((arith) 0);
	C_lae_dlb(con_label, (arith) 0);
	C_zer(pointer_size);
	C_lxa((arith) 0);
	C_cal("_ini");
	C_asp(4 * pointer_size);
}
