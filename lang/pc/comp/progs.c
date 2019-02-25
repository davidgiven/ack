#include    "parameters.h"
#include	"debug.h"

#include	<em.h>
#include	<assert.h>

#include	"progs.h"
#include	"LLlex.h"
#include	"def.h"
#include	"main.h"
#include	"scope.h"
#include	"type.h"

static int extflc;			/* number of external files */
static int inpflag = 0;			/* input mentioned in heading ? */
static int outpflag = 0;		/* output mentioned in heading ? */
static label extfl_label;		/* label of array of file pointers */

static void make_extfl_args();

void set_inp(void)
{
	inpflag = 1;
}

void set_outp(void)
{
	outpflag = 1;
}

void make_extfl(void)
{
	if( err_occurred ) return; 

	extfl_label = ++data_label;
	C_df_dlb(extfl_label);

	if( inpflag ) {
		C_ina_dnam(input);
		C_con_dnam(input, (arith) 0);
	}
	else
		C_con_ucon("0", pointer_size);

	if( outpflag ) {
		C_ina_dnam(output);
		C_con_dnam(output, (arith) 0);
	}
	else
		C_con_ucon("0", pointer_size);

	extflc = 2;

	/* Process the identifiers in the global scope (at this point only
	 * the program parameters) in order of specification.
	 */
	make_extfl_args( GlobalScope->sc_def );
}

static void make_extfl_args(register struct def *df)
{
	if( !df ) return;
	make_extfl_args(df->df_nextinscope);
	assert(df->df_flags & D_PROGPAR);
	if( df->var_name != input && df->var_name != output ) {
		C_ina_dnam(df->var_name);
		C_con_dnam(df->var_name, (arith) 0);
		extflc++;
	}
}

void call_ini(void)
{
	C_lxl((arith) 0);
	if( extflc )
		C_lae_dlb(extfl_label, (arith) 0);
	else
		C_zer(pointer_size);
	C_loc((arith) extflc);
	C_lxa((arith) 0);
	C_cal("_ini");
	C_asp(3 * pointer_size + word_size);
}
