/* D E S I G N A T O R   E V A L U A T I O N */

#ifndef NORCSID
static char *RcsId = "$Header$";
#endif

/*	Code generation for designators.
   	This file contains some routines that generate code common to address
	as well as value computations, and leave a description in a "desig"
	structure.  It also contains routines to load an address, load a value
	or perform a store.
*/

#include	"debug.h"

#include	<em_arith.h>
#include	<em_label.h>
#include	<assert.h>

#include	"type.h"
#include	"def.h"
#include	"scope.h"
#include	"desig.h"
#include	"LLlex.h"
#include	"node.h"

CodeValue(ds, size)
	register struct desig *ds;
{
	/*	Generate code to load the value of the designator described
		in "ds"
	*/

	switch(ds->dsg_kind) {
	case DSG_LOADED:
		break;

	case DSG_FIXED:
		if (size == word_size) {
			if (ds->dsg_name) {
				C_loe_dnam(ds->dsg_name, ds->dsg_offset);
			}
			else	C_lol(ds->dsg_offset);
			break;
		}

		if (size == dwird_size) {
			if (ds->dsg_name) {
				C_lde_dnam(ds->dsg_name, ds->dsg_offset);
			}
			else	C_ldl(ds->dsg_offset);
			break;
		}
		/* Fall through */
	case DSG_PLOADED:
	case DSG_PFIXED:
		CodeAddress(ds);
		C_loi(size);
		break;

	case DSG_INDEXED:
		C_lar(word_size);
		break;

	default:
		assert(0);
	}

	ds->dsg_kind = DSG_LOADED;
}

CodeAddress(ds)
	register struct desig *ds;
{
	/*	Generate code to load the address of the designator described
	   	in "ds"
	*/

	switch(ds->dsg_kind) {
	case DSG_PLOADED:
		if (ds->dsg_offset) {
			C_adp(ds->dsg_offset);
		}
		break;

	case DSG_FIXED:
		if (ds->dsg_name) {
			C_lae_dnam(ds->dsg_name, ds->dsg_offset);
			break;
		}
		C_lal(ds->dsg_offset);
		break;
		
	case DSG_PFIXED:
		ds->dsg_kind = DSG_FIXED;
		CodeValue(ds, pointer_size);
		break;

	case DSG_INDEXED:
		C_aar(word_size);
		break;

	default:
		assert(0);
		break;
	}

	ds->dsg_offset = 0;
	ds->dsg_kind = DSG_PLOADED;
}

CodeFieldDesig(df, ds)
	register struct def *df;
	register struct desig *ds;
{
	/* Generate code for a field designator. Only the code common for
	   address as well as value computation is generated, and the
	   resulting information on where to find the designator is placed
	   in "ds". "df" indicates the definition of the field.
	*/

	register struct withdesig *wds;

	if (ds->dsg_kind == DSG_INIT) {
		/* In a WITH statement. We must find the designator in the
		   WITH statement, and act as if the field is a selection
		   of this designator.
		   So, first find the right WITH statement, which is the
		   first one of the proper record type.
		   Notice that the proper record type is recognized by its
		   scope indication.
		*/
		wds = WithDesigs;
		assert(wds != 0);

		while (wds->w_scope != df->df_scope) {
			wds = wds->w_next;
			assert(wds != 0);
		}

		/* Found it. Now, act like it was a selection.
		*/
		*ds = wds->w_desig;
	}

	switch(ds->dsg_kind) {
	case DSG_PLOADED:
	case DSG_FIXED:
		ds->dsg_offset += df->fld_off;
		break;

	case DSG_PFIXED:
	case DSG_INDEXED:
		CodeAddress(ds);
		ds->dsg_kind = PLOADED;
		ds->dsg_offset = df->fld_off;
		break;

	default:
		assert(0);
		break;
	}
}

