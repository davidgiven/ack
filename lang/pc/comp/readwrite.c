/* R E A D ( L N )   &   W R I T E ( L N ) */

#include    "parameters.h"
#include	"debug.h"

#include	<assert.h>
#include	<em.h>

#include	"print.h"
#include	"LLlex.h"
#include	"def.h"
#include	"main.h"
#include	"misc.h"
#include	"node.h"
#include	"scope.h"
#include	"type.h"
#include	"code.h"
#include	"chk_expr.h"
#include	"typequiv.h"
#include	"error.h"
#include	"readwrite.h"


/* DEBUG */
#include	"idf.h"




/* Internal function prototypes */
static int ChkWriteParameter(struct type *, struct node *, char *);
static void CodeRead(register struct node *, register struct node *);
static void CodeRead(register struct node *, register struct node *);
static void CodeReadln(struct node *);
static void CodeWrite(register struct node *, register struct node *);
static void CodeWriteln(register struct node *);

void ChkRead(register struct node *arg)
{
	struct node *file;
	char *name = "read";
	char *message, buff[80];


	assert(arg);
	assert(arg->nd_symb == ',');

	if( arg->nd_left->nd_type->tp_fund == T_FILE )	{
		file = arg->nd_left;
		arg = arg->nd_right;
		if( !arg )	{
			error("\"%s\": variable-access expected", name);
			return;
		}
		MarkUsed(file);
	}
	else if( !(file = ChkStdInOut(name, 0)) )
		return;

	while( arg )	{
		assert(arg->nd_symb == ',');

		if( file->nd_type != text_type )	{
					/* real var & file of integer */
			if( !TstAssCompat(arg->nd_left->nd_type,
					BaseType(file->nd_type->next)) ) {
				node_error(arg->nd_left,
					"\"%s\": illegal parameter type",name);
				return;
			}
			else if( (BaseType(file->nd_type->next) == long_type
				    && arg->nd_left->nd_type == int_type)
				||
				(BaseType(file->nd_type->next) == int_type
				    && arg->nd_left->nd_type == long_type) ) {
			    if( int_size != long_size ) {
				 node_error(arg->nd_left,
					"\"%s\": longs and integers have different sizes",name);
				    return;
			    }
			    else node_warning(arg->nd_left,
					"\"%s\": mixture of longs and integers", name);
			}
		}
		else if( !(BaseType(arg->nd_left->nd_type)->tp_fund &
					( T_CHAR | T_NUMERIC )) )	{
			node_error(arg->nd_left,
					"\"%s\": illegal parameter type",name);
			return;
		}
		message = ChkAllowedVar(arg->nd_left, 1);
		if( message ) {
			sprint(buff,"\"%%s\": %s can't be a variable parameter",
							    message);
			node_error(arg->nd_left, buff, name);
			return;
		}

		CodeRead(file, arg->nd_left);
		arg = arg->nd_right;
	}
}

void ChkReadln(register struct node *arg)
{
	struct node *file;
	char *name = "readln";
	char *message, buff[80];


	if( !arg )	{
		if( !(file = ChkStdInOut(name, 0)) )
			return;
		else	{
			CodeReadln(file);
			return;
		}
	}

	assert(arg->nd_symb == ',');

	if( arg->nd_left->nd_type->tp_fund == T_FILE )	{
		if( arg->nd_left->nd_type != text_type )	{
			node_error(arg->nd_left,
					"\"%s\": textfile expected", name);
			return;
		}
		else	{
			file = arg->nd_left;
			arg = arg->nd_right;
			MarkUsed(file);
		}
	}
	else if( !(file = ChkStdInOut(name, 0)) )
		return;

	while( arg )	{
		assert(arg->nd_symb == ',');

		if( !(BaseType(arg->nd_left->nd_type)->tp_fund &
					( T_CHAR | T_NUMERIC )) )	{
			node_error(arg->nd_left,
					"\"%s\": illegal parameter type",name);
			return;
		}
		message = ChkAllowedVar(arg->nd_left, 1);
		if( message ) {
			sprint(buff,"\"%%s\": %s can't be a variable parameter",
							    message);
			node_error(arg->nd_left, buff, name);
			return;
		}
		CodeRead(file, arg->nd_left);
		arg = arg->nd_right;
	}
	CodeReadln(file);
}

void ChkWrite(register struct node *arg)
{
	struct node *left, *expp, *file;
	char *name = "write";

	assert(arg);
	assert(arg->nd_symb == ',');
	assert(arg->nd_left->nd_symb == ':');

	left = arg->nd_left;
	expp = left->nd_left;

	if( expp->nd_type->tp_fund == T_FILE )	{
		if( left->nd_right )	{
			node_error(expp,
			       "\"%s\": filevariable can't have a width",name);
			return;
		}
		file = expp;
		MarkUsed(file);
		arg = arg->nd_right;
		if( !arg )	{
			error("\"%s\": expression expected", name);
			return;
		}
	}
	else if( !(file = ChkStdInOut(name, 1)) )
		return;

	while( arg )	{
		assert(arg->nd_symb == ',');

		if( !ChkWriteParameter(file->nd_type, arg->nd_left, name) )
			return;

		CodeWrite(file, arg->nd_left);
		arg = arg->nd_right;
	}
}

void ChkWriteln(register struct node *arg)
{
	struct node *left, *expp, *file;
	char *name = "writeln";

	if( !arg )	{
		if( !(file = ChkStdInOut(name, 1)) )
			return;
		else	{
			CodeWriteln(file);
			return;
		}
	}

	assert(arg->nd_symb == ',');
	assert(arg->nd_left->nd_symb == ':');

	left = arg->nd_left;
	expp = left->nd_left;

	if( expp->nd_type->tp_fund == T_FILE )	{
		if( expp->nd_type != text_type )	{
			node_error(expp, "\"%s\": textfile expected", name);
			return;
		}
		if( left->nd_right )	{
			node_error(expp,
			      "\"%s\": filevariable can't have a width", name);
			return;
		}
		file = expp;
		MarkUsed(file);
		arg = arg->nd_right;
	}
	else if( !(file = ChkStdInOut(name, 1)) )
		return;

	while( arg )	{
		assert(arg->nd_symb == ',');

		if( !ChkWriteParameter(text_type, arg->nd_left, name) )
			return;

		CodeWrite(file, arg->nd_left);
		arg = arg->nd_right;
	}
	CodeWriteln(file);
}

static int ChkWriteParameter(struct type *filetype, struct node *arg, char *name)
{
	struct type *tp;
	char *mess = "illegal write parameter";

	assert(arg->nd_symb == ':');

	tp = BaseType(arg->nd_left->nd_type);

	if( filetype == text_type )	{
		if( !(tp == bool_type ||
				tp->tp_fund & (T_CHAR | T_NUMERIC | T_STRING) ||
				IsString(tp)) )	{
			node_error(arg->nd_left, "\"%s\": %s", name, mess);
			return 0;
		}
	}
	else	{
		if( !TstAssCompat(BaseType(filetype->next), tp) )	{
			node_error(arg->nd_left, "\"%s\": %s", name, mess);
			return 0;
		}
		if( arg->nd_right )	{
			node_error(arg->nd_left, "\"%s\": %s", name, mess);
			return 0;
		}
		else
			return 1;
	}

	/* Here we have a text-file */

	if( (arg = arg->nd_right) !=0 )	{
		/* Total width */

		assert(arg->nd_symb == ':');
		if( BaseType(arg->nd_left->nd_type) != int_type )	{
			node_error(arg->nd_left, "\"%s\": %s", name, mess);
			return 0;
		}
	}
	else
		return 1;

	if( (arg = arg->nd_right)!=0 )	{
		/* Fractional Part */

		assert(arg->nd_symb == ':');
		if( tp != real_type )	{
			node_error(arg->nd_left, "\"%s\": %s", name, mess);
			return 0;
		}
		if( BaseType(arg->nd_left->nd_type) != int_type )	{
			node_error(arg->nd_left, "\"%s\": %s", name, mess);
			return 0;
		}
	}
	return 1;
}

struct node *ChkStdInOut(char *name, int st_out)
{
	register struct def *df;
	register struct node *nd;

	if( !(df = lookup(str2idf(st_out ? output : input, 0),
			    GlobalScope, D_INUSE)) ||
			!(df->df_flags & D_PROGPAR) )	{
		error("\"%s\": standard input/output not defined", name);
		return NULLNODE;
	}

	nd = MkLeaf(Def, &dot);
	nd->nd_def = df;
	nd->nd_type = df->df_type;
	df->df_flags |= D_USED;

	return nd;
}

static void CodeRead(register struct node *file, register struct node *arg)
{
	struct type *tp = BaseType(arg->nd_type);

	if( err_occurred ) return;

	CodeDAddress(file);

	if( file->nd_type == text_type )	{
		switch( tp->tp_fund )	{
			case T_CHAR:
				C_cal("_rdc");
				break;

			case T_INTEGER:
				C_cal("_rdi");
				break;

			case T_LONG:
				C_cal("_rdl");
				break;

			case T_REAL:
				C_cal("_rdr");
				break;

			default:
				crash("(CodeRead)");
				/*NOTREACHED*/
		}
		C_asp(pointer_size);
		C_lfr(tp->tp_size);
		RangeCheck(arg->nd_type, file->nd_type->next);
		CodeDStore(arg);
	}
	else	{
		/* Keep the address of the file on the stack */
		C_dup(pointer_size);

		C_cal("_wdw");
		C_asp(pointer_size);
		C_lfr(pointer_size);
		RangeCheck(arg->nd_type, file->nd_type->next);

		C_loi(file->nd_type->next->tp_psize);
		if( tp == real_type ) {
		    if( BaseType(file->nd_type->next) == int_type ||
			BaseType(file->nd_type->next) == long_type )
			    Int2Real(file->nd_type->next->tp_psize);
		}

		CodeDStore(arg);
		C_cal("_get");
		C_asp(pointer_size);
	}
}

static void CodeReadln(struct node *file)
{
	if( err_occurred ) return;

	CodeDAddress(file);
	C_cal("_rln");
	C_asp(pointer_size);
}

static void CodeWrite(register struct node *file, register struct node *arg)
{
	int width = 0;
	register arith nbpars = pointer_size;
	register struct node *expp = arg->nd_left;
	struct node *right = arg->nd_right;
	struct type *tp = BaseType(expp->nd_type);

	if( err_occurred ) return;

	CodeDAddress(file);
	CodePExpr(expp);

	if( file->nd_type == text_type )	{
		if( tp->tp_fund & (T_ARRAY | T_STRINGCONST) )	{
			C_loc(IsString(tp));
			nbpars += pointer_size + int_size;
		}
		else nbpars += tp->tp_size;

		if( right )	{
			width = 1;
			CodePExpr(right->nd_left);
			nbpars += int_size;
			right = right->nd_right;
		}

		switch( tp->tp_fund )	{
			case T_ENUMERATION:	/* boolean */
				C_cal(width ? "_wsb" : "_wrb");
				break;

			case T_CHAR:
				C_cal(width ? "_wsc" : "_wrc");
				break;

			case T_INTEGER:
				C_cal(width ? "_wsi" : "_wri");
				break;

			case T_LONG:
				C_cal(width ? "_wsl" : "_wrl");
				break;

			case T_REAL:
				if( right )	{
					CodePExpr(right->nd_left);
					nbpars += int_size;
					C_cal("_wrf");
				}
				else C_cal(width ? "_wsr" : "_wrr");
				break;

			case T_ARRAY:
			case T_STRINGCONST:
				C_cal(width ? "_wss" : "_wrs");
				break;

			case T_STRING:
				C_cal(width ? "_wsz" : "_wrz");
				break;

			default:
				crash("(CodeWrite)");
				/*NOTREACHED*/
		}
		C_asp(nbpars);
	}
	else	{
		if( file->nd_type->next == real_type && tp == int_type )
			Int2Real(int_size);
		else if( file->nd_type->next == real_type && tp == long_type )
			Int2Real(long_size);

		CodeDAddress(file);
		C_cal("_wdw");
		C_asp(pointer_size);
		C_lfr(pointer_size);
		C_sti(file->nd_type->next->tp_psize);

		C_cal("_put");
		C_asp(pointer_size);
	}
}

static void CodeWriteln(register struct node *file)
{
	if( err_occurred ) return;

	CodeDAddress(file);
	C_cal("_wln");
	C_asp(pointer_size);
}
