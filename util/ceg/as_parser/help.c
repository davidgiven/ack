#include "varargs.h"
#include "decl.h"

/* All the functions in this file will be called by the parser.
 */

extern char	*strindex();

static struct Op_info { char *name, *type; }
			op_info[ MAX_OPERANDS] = { { 0, 0}};

static int	n_ops = 0;		/* Number of opertands of current
					 * assembly instruction.
					 */
static char	*assem_instr = 0;	/* Name of the current assembly instr */
static Bool	restriction = FALSE;	/* Is there a restriction on the
					 * current operand?
					 */
File *outfile;

save_instr( instr, len)
char *instr;
int len;
{
	assem_instr = Salloc( instr, len + 1);
}

save_name( name, len)
char *name;
int len;
{
	op_info[ n_ops].name = Salloc( name, len + 1);
}

save_type( type, len)
char *type;
int len;
{
	op_info[ n_ops].type = Salloc( type, len + 1);
	restriction = TRUE;
}

pr_header()
{
	out( "%s_instr", assem_instr);
	param_list();
	out( "{\n");
	save_mnem( assem_instr);
}

param_list()
{
	int i;

	out( "(");
	if ( n_ops > 0)	{
		out( " %s", op_info[0].name);
		for ( i = 1; i < n_ops; i++) 
			out( ", %s", op_info[i].name);
	}

	out( ")\n");
	if ( n_ops > 0)	{
		out( "struct t_operand *%s", op_info[0].name);
		for ( i = 1; i < n_ops; i++) 
			out( ", *%s", op_info[i].name);
		out( ";\n");
	}
}


pr_restriction()
{
	int i;
	Bool more = FALSE;

	if ( !restriction)
		return;

	out( "if ( ");
	for ( i = 0; i < n_ops; i++)
		if ( op_info[i].type != 0) {
			if ( more)
				out( " &&");
			out( " %s( %s)", op_info[i].type, op_info[i].name);
			more = TRUE;
		}
	out( ") ");
}

pr_warning()
{
	if ( restriction)
		out( "else\nerror( \"No match for %s\");\n", assem_instr);
	restriction = FALSE;
}

clear_restriction()
{
	restriction = FALSE;
}

char *skip_string( str)
char *str;

/* returns position after the first '"'-charcter, look out for '\' escape
 * sequence
 */
{
	for ( str++; *str != '"' || *(str-1) == '\\'; str++);
	return( str + 1);
}

pr_subroutine( str)
char *str;
{
	out( "%s;\n", str);
}


#include <ctype.h>

pr_call( str)
char *str;

/* Ouput 'str', but keep track of the number of bytes and take care of
 * conversions like %$.
 */
{
	if ( strncmp( "text", str, 4) == 0 && isdigit( *(str+4))) 
		out( "cur_pos += %d;\n", *(str+4) - '0');
	else if ( strncmp( "reloc", str, 5) == 0 && isdigit( *(str+5)))
		out( "cur_pos += %d;\n", *(str+5) - '0');

	pr_text_with_conversions( str);
	out( "fprint( outfile, \";\");");
}

pr_end()
{
	out( "fprint( outfile, \"}\\n\");");
}

pr_els()
{
	out( "fprint( outfile, \"else\\n\");");
}

pr_else()
{
	out( "fprint( outfile, \"else {\\n\");");
}

pr_question( quest)
char *quest;
{
	out( "fprint( outfile, \"if\");");
        pr_text_with_conversions( quest);
	out( "fprint( outfile, \"{\\n\");");
}


init_table()
{
	outfile = STDOUT;
	out( "#include \"as.h\"\n");
	out( "#include \"as_parser.h\"\n");
}

clean()

/* Free space, allocated during the parsing of an entry in 'as_table'.
 */
{
	int i;

	if ( assem_instr != 0) {
		free( assem_instr);
		assem_instr = 0;
	}

	for ( i = 0; i < n_ops; i++) {
		free( op_info[i].name);
		op_info[i].name = 0;
		if ( op_info[i].type != 0) {
			free( op_info[i].type);
			op_info[i].type = 0;
		}
	}
	n_ops = 0;
}

operand_clean()

/* Free space for the operands */

{
	int i;

	for ( i = 0; i < n_ops; i++) {
		free( op_info[i].name);
		op_info[i].name = 0;
		if ( op_info[i].type != 0) {
			free( op_info[i].type);
			op_info[i].type = 0;
		}
	}
	n_ops = 0;
}

/*VARARGS*/
out(va_alist)
va_dcl
{
	va_list pvar;
	char *fmt;

	va_start(pvar);
	fmt = va_arg(pvar, char *);
	doprnt( outfile, fmt, pvar);
	va_end(pvar);
}

extern int nerrors;

/*VARARGS*/
error(va_alist)
va_dcl
{
	char *fmt;
	va_list pvar;

	nerrors++;
	va_start(pvar);
	fmt = va_arg(pvar, char *);
	fprint( STDERR, "!! ERROR :	");
	doprnt( STDERR, fmt, pvar);
	fprint( STDERR, "	!!\n");
	va_end(pvar);
}

inc_ops()
{
	n_ops++;
}


/**********************************/

char *mnemonic[ MAX_MNEMONICS];
int n_mnems = 0;

save_mnem( mnem)
char *mnem;
{
	if ( n_mnems == MAX_MNEMONICS)
		error( "too many assembler instructions!! MAX_MNEMONICS = %d",
			MAX_MNEMONICS);
	else
		mnemonic[ n_mnems++] = Salloc( mnem, strlen( mnem) + 1);
}


end_table()

/* Flush information in the array 'mnemonic'
 */
{
	int i;

	quicksort( 0, n_mnems - 1);
	
	out( "char *mnemonic[] = {\n");
	for ( i = 0; i < n_mnems - 1; i++)
		out( "\t\"%s\",\n", mnemonic[i]);
	out( "\t\"%s\"};\n\n", mnemonic[ n_mnems-1]);

	out( "int (*instruction[])() = {\n");
	for ( i = 0; i < n_mnems - 1; i++)
		out( "\t%s_instr,\n", mnemonic[i]);
	out( "\t%s_instr};\n\n", mnemonic[ n_mnems-1]);
	out( "int n_mnems = %d;\n", n_mnems);
}

quicksort( lower, upper)
int lower, upper;

/* Sort the array 'mnemonic'.
 */
{
	char *key, *tmp;
	int index1, index2;

	if ( lower >= upper)
		return;

	key = mnemonic[lower];
	index1 = lower;
	index2 = upper+1;

	while ( index1 < index2) {

		do
			index1++;
		while (index1 <= upper && strcmp( mnemonic[index1], key) < 0 );

		do
			index2--;
		while ( strcmp( mnemonic[index2], key) > 0);

		if ( index1 < index2) {
			tmp = mnemonic[index2];
			mnemonic[index2] = mnemonic[index1];
			mnemonic[index1] = tmp;
		}
	}

	mnemonic[lower] = mnemonic[index2];
	mnemonic[index2] = key;

	quicksort( lower, index2-1);
	quicksort( index2+1, upper);
}
