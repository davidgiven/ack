/* $Header$ */

/* Symbol table reader
*/

{
#include	<alloc.h>
#include	<stb.h>
#include	<assert.h>

#include	"position.h"
#include	"file.h"
#include	"type.h"
#include	"symbol.h"
#include	"scope.h"
#include	"class.h"
#include	"idf.h"
#include	"rd.h"

extern char	*strindex();
extern long	str2long();
extern double	atof();

static char	*DbPtr;			/* current pointer in db string */
static int	AllowName;		/* set if NAME legal at this point */
static long	ival;
static double	fval;
static char	*strval;
static int	last_index[2];
static struct outname	*currnam;
static int	saw_code;

static struct literal *get_literal_space();
static struct fields *get_field_space();
static end_field();
static char *string_val();
}

%start DbParser, debugger_string;

%prefix DBS;

%lexical DBSlex;

%onerror DBSonerror;

%token	INTEGER, REAL, STRING, NAME;

debugger_string
  { register p_symbol s;
    char *str;
    p_type tmp = 0;
    int upb = 0;
  }
:
  name(&str)
  [ /* constant name */
			{ s = NewSymbol(str, CurrentScope, CONST, currnam); }
	'c' const_name(s)

  | /* type name */
			{ s = NewSymbol(str, CurrentScope, TYPE, currnam); }
	't' type_name(&(s->sy_type), s)
			{ if (! s->sy_type->ty_sym) s->sy_type->ty_sym = s; 
			  if ((s->sy_type->ty_class == T_ENUM ||
			       s->sy_type->ty_class == T_SUBRANGE) &&
			      currnam->on_desc != 0) {
				s->sy_type->ty_size = currnam->on_desc;
			  }
			}
  | /* tag name (only C?) */
			{ s = NewSymbol(str, CurrentScope, TAG, currnam); }
	'T' type_name(&(s->sy_type), s)
			{ if (! s->sy_type->ty_sym) s->sy_type->ty_sym = s; 
			  if (s->sy_type->ty_class != T_CROSS) {
				resolve_cross(s->sy_type);
			  }
			}
  | /* end scope */
	'E' INTEGER
			{ close_scope(); }

  | /* module begin */
			{ s = NewSymbol(str, CurrentScope, MODULE, currnam); }
	'M' INTEGER
			{ open_scope(s, 1);
			  s->sy_name.nm_scope = CurrentScope;
			  CurrentScope->sc_start = currnam->on_valu;
			  CurrentScope->sc_proclevel = currnam->on_desc;
			  add_scope_addr(CurrentScope);
			}

  | /* external procedure */
			{ s = NewSymbol(str, FileScope, PROC, currnam); }
	'P' routine(s)

  | /* private procedure */
			{ s = NewSymbol(str, CurrentScope, PROC, currnam); }
	'Q' routine(s)

  | /* external function */
			{ s = NewSymbol(str, FileScope, FUNCTION, currnam); }
	'F' function(s)

  | /* private function */
			{ s = NewSymbol(str, CurrentScope, FUNCTION, currnam); }
	'f' function(s)

  | /* global variable, external */
				/* maybe we already know it; but we need
				   the type information anyway for other
				   types.
				*/
			{ s = Lookup(findidf(str), FileScope, VAR);
			  if (s) {
				tmp = s->sy_type;
				s->sy_type = 0;
			  } else s = NewSymbol(str, FileScope, VAR, currnam);
			}
	'G' type(&(s->sy_type), (int *) 0, s)
			{ if (tmp) s->sy_type = tmp; } 

  | /* static variable */
			{ s = NewSymbol(str, CurrentScope, VAR, currnam); }
	'S' type(&(s->sy_type), (int *) 0, s)

  | /* static variable, local scope */
			{ s = NewSymbol(str, CurrentScope, VAR, currnam); }
	'V' type(&(s->sy_type), (int *) 0, s)

  | /* register variable */
			{ s = NewSymbol(str, CurrentScope, REGVAR, currnam); }
	'r' type(&(s->sy_type), (int *) 0, s)

  | /* value parameter */
			{ s = NewSymbol(str, CurrentScope, LOCVAR, currnam); }
	'p' type(&(s->sy_type), (int *) 0, s)
			{ add_param_type('p', s); }

  | /* value parameter but address passed */
			{ s = NewSymbol(str, CurrentScope, VARPAR, currnam); }
	'i' type(&(s->sy_type), (int *) 0, s)
			{ add_param_type('i', s); }

  | /* variable parameter */
			{ s = NewSymbol(str, CurrentScope, VARPAR, currnam); }
	'v' type(&(s->sy_type), (int *) 0, s)
			{ add_param_type('v', s); }

  | /* local variable */
			{ s = NewSymbol(str, CurrentScope, LOCVAR, currnam); }
	type_name(&(s->sy_type), s)

  | /* lower or upper bound of array descriptor */
	[ 'A' 		{ upb = LBOUND; }
	| 'Z'		{ upb = UBOUND; }
	]
	[ ['p' | ]	{ s = NewSymbol(str, CurrentScope, LOCVAR, currnam);
			  if (upb == UBOUND) add_param_type('Z', s);
			}
	| [ 'V' | 'S' ]	{ s = NewSymbol(str, CurrentScope, VAR, currnam); }
	]
	type_name(&(s->sy_type), s)
			{ p_symbol s1 = new_symbol();
			  *s1 = *s;
			  s->sy_class = upb;
			  s->sy_descr = s1;
			}

  | /* function result in Pascal; ignore ??? */
			{ s = NewSymbol(str, CurrentScope, LOCVAR, currnam); }
	'X' type_name(&(s->sy_type), s)
  ]
  ';'?
;

name(char **s;)
:
  /* anything up to a ':' */
  NAME	{ *s = strval; }
;

const_name(p_symbol cst;)
  { int type_index[2];
    long iconst;
    register char *p;
  }
:
  '='
  [
/*
	'b' integer_const(&(cst->sy_const.co_ival))	/* boolean */
/*  |
*/
	'c' integer_const(&(cst->sy_const.co_ival))	/* character */
				{ cst->sy_type = char_type; }
  |
	'i' integer_const(&(cst->sy_const.co_ival))	/* integer */
				{ cst->sy_type = long_type; }
  |
  	'r' real_const(&(cst->sy_const.co_rval))	/* real */
				{ cst->sy_type = double_type; }
  |
	's' string_const				/* string */
				{ cst->sy_const.co_sval = string_val(strval);
				  cst->sy_type = string_type;
				}
  |
	'e' type_index(type_index) ',' integer_const(&(cst->sy_const.co_ival))
				/* enumeration constant;
				 * enumeration type, value
				 */
				{ cst->sy_type = *tp_lookup(type_index); }
				
  |
	'S' type_index(type_index)
				{ cst->sy_type = *tp_lookup(type_index);
				  cst->sy_const.co_setval = p =
				    Malloc((unsigned) cst->sy_type->ty_size);
				}
	[ ',' integer_const(&iconst)
				{ *p++ = iconst; }
	]+
				/* set constant:
				 *  settype, values of the bytes
				 *  in the set.
				 */
  ]
;

integer_const(long *iconst;)
  { int sign = 0; }
:
  [ '+' | '-' { sign = 1; } ]?
  INTEGER			{ *iconst = sign ? -ival : ival; }
;

real_const(double *f;)
  { int sign = 0; }
:
  [ '+' | '-' { sign = 1; } ]?
  REAL				{ *f = sign ? fval : -fval; }
;

string_const
:
  STRING			/* has SINGLE quotes! */
;

type_name(p_type *t; p_symbol sy;)
  { int type_index[2]; p_type *p; }
:
  type_index(type_index)	{ p = tp_lookup(type_index); }
  [
				{ if (*p && (*p)->ty_class != 0 &&
				      (*p)->ty_class != T_CROSS) {
					error("Redefining (%d,%d) %d",
					  type_index[0],
					  type_index[1],
					  (*p)->ty_class);
				  }
				}
	'='			
	type(p, type_index, sy)
  |
				{ if (*t && ! *p) *p = *t; 
				  else if (*t) **t = **p;
				}
  ]
				{ if (*p == 0) *p = new_type();
				  *t = *p;
				}
;

type_index(int *type_index;)
:
[
  INTEGER			{ type_index[0] = 0; type_index[1] = ival; }
|
  '(' INTEGER			{ type_index[0] = ival; }
  ',' INTEGER			{ type_index[1] = ival; }
  ')'
]
				{ last_index[0] = type_index[0];
				  last_index[1] = type_index[1];
				}
;

function(p_symbol p;)
:
  			{ p->sy_type = new_type();
			  p->sy_type->ty_class = T_PROCEDURE;
			  p->sy_type->ty_size = pointer_size;
			}
  type(&(p->sy_type->ty_retval), (int *) 0, (p_symbol) 0) 
  			{ if (CurrentScope != FileScope &&
			      saw_code) {
				/* if saw_code is not set, it is a nested
				   procedure
				*/
				close_scope();
			  }
			  saw_code = 0;
			  open_scope(p, 1);
			  p->sy_name.nm_scope = CurrentScope;
			  CurrentScope->sc_start = currnam->on_valu;
			  add_scope_addr(CurrentScope);
			  CurrentScope->sc_proclevel = currnam->on_desc;
			}
;

routine(p_symbol p;)
:
  			{ p->sy_type = new_type();
			  p->sy_type->ty_class = T_PROCEDURE;
			  p->sy_type->ty_size = pointer_size;
			  if (CurrentScope != FileScope &&
			      saw_code) {
				/* if saw_code is not set, it is a nested
				   procedure
				*/
				close_scope();
			  }
			  saw_code = 0;
			  open_scope(p, 1);
			  p->sy_name.nm_scope = CurrentScope;
			  CurrentScope->sc_start = currnam->on_valu;
			  add_scope_addr(CurrentScope);
			  CurrentScope->sc_proclevel = currnam->on_desc;
			}
  INTEGER ';'
  type(&(p->sy_type->ty_retval), (int *) 0, (p_symbol) 0) 
;

type(p_type *ptp; int *type_index; p_symbol sy;)
  { register p_type tp = *ptp ? *ptp : new_type();
    p_type t1 = 0, t2 = 0;
    long ic1, ic2;
    int A_used = 0;
    int tclass;
    int tp_index[2];
    char *str;
  }
:
  [
	/* type cross reference */
	/* these are used in C for references to a struct, union or
	 * enum that has not been declared (yet)
	 */
  	'x'
  	[ 's'	/* struct */
			{ tclass = T_STRUCT; }
  	| 'u'	/* union */
			{ tclass = T_UNION; }
  	| 'e'	/* enum */
			{ tclass = T_ENUM; }
  	]
			{ AllowName = 1; }
  	name(&str)
			{ sy = Lookfromscope(str2idf(str,0),TAG,CurrentScope);
			  if (sy && 
			      (sy->sy_type->ty_class == tclass ||
			       sy->sy_type->ty_class == T_CROSS)) {
				if (tp != *ptp) free_type(tp);
				tp = sy->sy_type;
			  }
			  else {
				tp->ty_class = T_CROSS;
				tp->ty_size = tclass;
				sy = NewSymbol(str, CurrentScope, TAG, (struct outname *) 0);
				sy->sy_type = tp;
			  }
			}
  |
  	/* subrange */
  	/* the integer_const's represent the lower and the upper bound.
   	 * A subrange type defined as subrange of itself is an integer type.
   	 * If the second integer_const == 0, but the first is not, we
   	 * have a floating point type with size equal to the first
   	 * integer_const.
   	 * Upperbound -1 means unsigned int or unsigned long.
   	 */
  	'r' type_index(tp_index) ';'
	[ 'A' integer_const(&ic1)	{ A_used = 1; }
	| integer_const(&ic1)
	]
	';'
	[ 'A' integer_const(&ic2)	{ A_used |= 2; }
	| integer_const(&ic2)
	| 'Z' integer_const(&ic2)	{ A_used |= 0200; }
	]
			{ if (tp != *ptp) free_type(tp);
			  tp = subrange_type(A_used,
					       tp_index,
					       ic1,
					       ic2,
					       type_index);
			}
  |
  	/* array; first type is bound type, next type
   	 * is element type
   	 */
  	'a' type(&t1, (int *) 0, (p_symbol) 0) ';' type(&t2, (int *) 0, (p_symbol) 0)
			{ if (tp != *ptp) free_type(tp);
			  tp = array_type(t1, t2); 
			}
  |
  	/* structure type */
  	's'		{ tp->ty_class = T_STRUCT; }
	structure_type(tp, sy)
  |
  	/* union type */
  	'u'		{ tp->ty_class = T_UNION; }
	structure_type(tp, sy)
  |
  	/* enumeration type */
  	'e'		{ tp->ty_class = T_ENUM; }
	enum_type(tp)
  |
  	/* pointer type */
  	'*'		{ tp->ty_class = T_POINTER;
			  tp->ty_size = pointer_size;
			}
  	type(&(tp->ty_ptrto), (int *) 0, (p_symbol) 0)
  |
  	/* function type */
  	'f'		{ tp->ty_class = T_PROCEDURE;
			  tp->ty_size = pointer_size;
			}
  	type(&(tp->ty_retval), (int *) 0, (p_symbol) 0) 
/*
  	[ %prefer
		',' param_list(tp)
  	|
  	]
*/
  |
  	/* procedure type */
  	'Q'		{ tp->ty_class = T_PROCEDURE;
			  tp->ty_size = pointer_size;
			}
  	type(&(tp->ty_retval), (int *) 0, (p_symbol) 0) 
	',' param_list(tp)
  |
  	/* another procedure type */
  	'p'		{ tp->ty_class = T_PROCEDURE;
			  tp->ty_size = pointer_size;
			  tp->ty_retval = void_type;
			}
	param_list(tp)
  |
  	/* set type */
  	/* the first integer_const represents the size in bytes,
   	 * the second one represents the low bound
   	 */
  	'S'		{ tp->ty_class = T_SET; }
	type(&(tp->ty_setbase), (int *) 0, (p_symbol) 0) ';'
	[
		integer_const(&(tp->ty_size)) ';'
		integer_const(&(tp->ty_setlow)) ';'
	|
			{ set_bounds(tp); }
	]
  |
	/* file type of Pascal */
	'L'		{ tp->ty_class = T_FILE; }
	type(&(tp->ty_fileof), (int *) 0, (p_symbol) 0)
  |
  	type_name(ptp, (p_symbol) 0)
			{ if (type_index &&
			      (*ptp)->ty_class == 0 &&
			      type_index[0] == last_index[0] &&
			      type_index[1] == last_index[1]) {
				**ptp = *void_type;
				if (*ptp != tp) free_type(tp);
			  }
			  tp = *ptp;
			}
  ]
			{ if (*ptp && *ptp != tp) **ptp = *tp;
			  else *ptp = tp;
			}
;

structure_type(register p_type tp; p_symbol sy;)
  { register struct fields *fldp;
    char *str;
  }
:
  integer_const(&(tp->ty_size))		/* size in bytes */
			{ open_scope(sy, 0);
			  if (sy) sy->sy_name.nm_scope = CurrentScope;
			}
  [
	name(&str)	{ fldp = get_field_space(tp, str); }
	type(&(fldp->fld_type), (int *) 0, (p_symbol) 0) ','
	integer_const(&(fldp->fld_pos)) ','	/* offset in bits */
	integer_const(&(fldp->fld_bitsize)) ';'	/* size in bits */
  ]*
  ';'			{ end_field(tp); 
			  close_scope();
			}
;

enum_type(register p_type tp;)
  { register struct literal *litp;
    long maxval = 0;
    register p_symbol s;
  }
:
  [			{ litp = get_literal_space(tp); }
	name(&(litp->lit_name))
	integer_const(&(litp->lit_val)) ',' 
			{ if (maxval < litp->lit_val) maxval = litp->lit_val;
			  AllowName = 1;
			  s = NewSymbol(litp->lit_name, CurrentScope, CONST, (struct outname *) 0);
			  s->sy_const.co_ival = litp->lit_val;
			  s->sy_type = tp;
			}
  ]*
  ';'			{ end_literal(tp, maxval); }
;

param_list(p_type t;)
  { register struct param *p;
    long iconst;
  }
:
  integer_const(&iconst) ';'	/* number of parameters */
			{ t->ty_nparams = iconst;
			  t->ty_params = p = (struct param *)
			    Malloc((unsigned)(t->ty_nparams * sizeof(struct param)));
			}
  [
  	[	'p'	{ p->par_kind = 'p'; }
  	|	'v'	{ p->par_kind = 'v'; }
  	|	'i' 	{ p->par_kind = 'i'; }
  	]
  	type(&(p->par_type), (int *) 0, (p_symbol) 0) ';'
			{ p->par_off = t->ty_nbparams;
			  t->ty_nbparams += 
				param_size(p->par_type, p->par_kind);
			  p++;
			}
  ]*
;

{
static char *db_string;
static char *DbOldPtr;

static struct outname *
DbString(n)
  struct outname	*n;
{
  currnam = n;
  DbPtr = n->on_mptr;
  db_string = DbPtr;
  AllowName = 1;
  DbParser();
  return currnam;
}

/*ARGSUSED*/
DBSmessage(n)
{
  fatal("error in symbol table string \"%s\", DbPtr = \"%s\", DbOldPtr = \"%s\"",
	db_string,
	DbPtr,
	DbOldPtr);

}

DBSonerror(tk, p)
  int	*p;
{
  DbPtr = DbOldPtr;
/* ???  if (DBSsymb < 0) {
	while (*p && *p != ';') p++;
	if (*p) DbPtr = ";";
	return;
  }
*/
  if (! tk) {
	while (*p && *p != NAME) p++;
	if (*p) {
		AllowName = 1;
	}
  }
  else if (tk == NAME) AllowName = 1;
}

DBSlex()
{
  register char *cp = DbPtr;
  int allow_name = AllowName;
  register int c;

  AllowName = 0;
  DbOldPtr = cp;
  c = *cp;
  if (c == '\\' && *(cp+1) == '\0') {
	currnam++;
	cp = currnam->on_mptr;
	DbOldPtr = cp;
	c = *cp;
  }
  if (! c) {
	DbPtr = cp;
	return -1;
  }
  if ((! allow_name && is_token(c)) || c == ';') {
	DbPtr = cp+1;
	return c;
  }
  if (is_dig(c)) {
	int retval = INTEGER;

	while (++cp, is_dig(*cp)) /* nothing */;
	c = *cp;
	if (c == '.') {
		retval = REAL;
		while (++cp, is_dig(*cp)) /* nothing */;
		c = *cp;
	}
	if (c == 'e' || c == 'E') {
		char *oldcp = cp;

		cp++;
		c = *cp;
		if (c == '-' || c == '+') {
			cp++;
			c = *cp;
		}
		if (is_dig(c)) {
			retval = REAL;
			while (++cp, is_dig(*cp)) /* nothing */;
		}
		else cp = oldcp;
	}
	c = *cp;
	*cp = 0;
	if (retval == INTEGER) {
		ival = str2long(DbOldPtr, 10);
	}
	else {
		fval = atof(DbOldPtr);
	}
	*cp = c;
	DbPtr = cp;
	return retval;
  }
  if (c == '\'') {
	cp++;
	strval = cp;
	while ((c = *cp) && c != '\'') {
		if (c == '\\') cp++;	/* backslash escapes next character */
		if (!(c =  *cp)) break;	/* but not a null byte */
		cp++;
	}
	if (! c) DBSmessage(0);	/* no return */
	*cp = 0;
	DbPtr = cp + 1;
	return STRING;
  }
  strval = cp;
  while ((c = *cp) && c != ':' && c != ',') cp++;
  DbPtr = *cp ? cp+1 : cp;
  *cp = 0;
  return NAME;
}

static struct fields *
get_field_space(tp, s)
  register p_type tp;
  char	*s;
{
  register struct fields *p;
  p_symbol	sy;

  if (! (tp->ty_nfields & 07)) {
	tp->ty_fields = (struct fields *)
		  Realloc((char *) tp->ty_fields,
			    (tp->ty_nfields+8)*sizeof(struct fields));
  }
  p = &tp->ty_fields[tp->ty_nfields++];
  p->fld_name = s;
  p->fld_type = 0;
  sy = NewSymbol(s, CurrentScope, FIELD, currnam);
  sy->sy_field = p;
  return p;
}

static
end_field(tp)
  register p_type tp;
{
  tp->ty_fields = (struct fields *)
	Realloc((char *) tp->ty_fields,
		tp->ty_nfields * sizeof(struct fields));
}

static struct literal *
get_literal_space(tp)
  register p_type tp;
{
  if (! (tp->ty_nenums & 07)) {
	tp->ty_literals = (struct literal *)
		Realloc((char *) tp->ty_literals,
			(tp->ty_nenums+8)*sizeof(struct literal));
  }
  return &tp->ty_literals[tp->ty_nenums++];
}

static char *
string_val(s)
  char	*s;
{
  register char *ns = s, *os = s;
  register unsigned int i = 1;

  for (;;) {
	if (!*os) break;
	i++;
	if (*os == '\\') {
		os++;
		*ns++ = *os++;
	}
	else *ns++ = *os++;
  }
  *ns = '\0';
  return Salloc(s, i);
}

static char		*AckStrings;	/* ACK a.out string table */
static struct outname	*AckNames;	/* ACK a.out symbol table entries */
static unsigned int	NAckNames;	/* Number of ACK symbol table entries */
static struct outname	*EndAckNames;	/* &AckNames[NAckNames] */

/* Read the symbol table from file 'f', which is supposed to be an
   ACK a.out format file. Offer db strings to the db string parser.
*/
int
DbRead(f)
  char	*f;
{
  struct outhead h;
  register struct outname *n;
  register struct outname *line_file = 0;
  long OffsetStrings;
  int lbrac_required = 0;
  int needs_newscope = 0;
  int lbrac_level = 0;

  /* Open file, read header, and check magic word */
  if (! rd_open(f)) {
  	fatal("%s: could not open", f);
  }
  rd_ohead(&h);
  if (BADMAGIC(h) && h.oh_magic != O_CONVERTED) {
  	fatal("%s: not an object file", f);
  }

  /* Allocate space for name table and read it */
  AckNames = (struct outname *) 
  		Malloc((unsigned)(sizeof(struct outname) * h.oh_nname));
  AckStrings = Malloc((unsigned) h.oh_nchar);
  rd_name(AckNames, h.oh_nname);
  rd_string(AckStrings, h.oh_nchar);

  /* Adjust file offsets in name table to point at strings */
  OffsetStrings = OFF_CHAR(h);
  NAckNames = h.oh_nname;
  EndAckNames = &AckNames[h.oh_nname];
  for (n = EndAckNames; --n >= AckNames;) {
	if (n->on_foff) {
		if ((unsigned)(n->on_foff - OffsetStrings) >= h.oh_nchar) {
			fatal("%s: error in object file", f);
		}
		n->on_mptr = AckStrings + (n->on_foff - OffsetStrings);
	}
	else	n->on_mptr = 0;
  }

  /* Offer strings to the db string parser if they contain a ':'.
     Also offer filename-line number information to add_position_addr().
     Here, the order may be important.
  */
  for (n = &AckNames[0]; n < EndAckNames; n++) {
	int tp = n->on_type >> 8;
	register p_symbol sym;

	if (tp & (S_STB >> 8)) {
		switch(tp) {
#ifdef N_BINCL
		case N_BINCL:
			n->on_valu = (long) line_file;
			line_file = n;
			break;
		case N_EINCL:
			if (line_file) {
				line_file = (struct outname *) line_file->on_valu;
			}
			break;
#endif
		case N_SO:
			if (n->on_mptr[strlen(n->on_mptr)-1] == '/') {
				/* another N_SO follows ... */
				break;
			}
			clean_tp_tab();
			while (CurrentScope != PervasiveScope) {
				close_scope();
			}
			saw_code = 0;
			sym = add_file(n->on_mptr);

			if (! listfile) newfile(sym->sy_idf);
			open_scope(sym, 0);
			sym->sy_file->f_scope = CurrentScope;
			FileScope = CurrentScope;
			CurrentScope->sc_start = n->on_valu;
			/* fall through */
		case N_SOL:
			if (! line_file) line_file = n;
			else line_file->on_mptr = n->on_mptr;
			break;
		case N_MAIN:
			if (! FileScope) fatal("No file scope");
			newfile(FileScope->sc_definedby->sy_idf);
			break;
		case N_SLINE:
			assert(line_file);
			if (CurrentScope->sc_start) {
				register p_scope sc =
					get_scope_from_addr(n->on_valu);

				if (sc) CurrentScope = sc;
			}
			if (! saw_code && !CurrentScope->sc_bp_opp) {
			    CurrentScope->sc_bp_opp = n->on_valu;
			    if (! CurrentScope->sc_start) {
				CurrentScope->sc_start = n->on_valu;
				add_scope_addr(CurrentScope);
			    }
			}
			saw_code = 1;
			add_position_addr(line_file->on_mptr, n);
			break;
		case N_LBRAC:	/* block, desc = nesting level */
			if (lbrac_level && ! lbrac_required) {
				open_scope((p_symbol) 0, 0);
				saw_code = 0;
			}
			else {
				/* Sun-4 ld does not relocate LBRAC
				   values, so we cannot use it
				
				register p_scope sc = 
					get_scope_from_addr(n->on_valu);

				if (!sc || sc->sc_bp_opp) {
				}
				else CurrentScope = sc;
				*/
			}
			lbrac_level++;
			needs_newscope = 1;
			break;
#ifdef N_SCOPE
		case N_SCOPE:
			if (n->on_mptr && strindex(n->on_mptr, ':')) {
				n = DbString(n);
			}
			break;
#endif
		case N_RBRAC:	/* end block, desc = nesting level */
			if (CurrentScope != FileScope) close_scope();
			needs_newscope = 1;
			if (--lbrac_level == 0) needs_newscope = 0;
			saw_code = 0;
			break;
		case N_FUN:	/* function, value = address */
		case N_GSYM:	/* global variable */
		case N_STSYM:	/* data, static, value = address */
		case N_LCSYM:	/* bss, static, value = address */
		case N_RSYM:	/* register var, value = reg number */
		case N_SSYM:	/* struct/union el, value = offset */
		case N_PSYM:	/* parameter, value = offset from AP */
		case N_LSYM:	/* local sym, value = offset from FP */
			if (needs_newscope) {
				open_scope((p_symbol) 0, 0);
				saw_code = 0;
				needs_newscope = 0;
				lbrac_required = 1;
			}
			if (n->on_mptr && strindex(n->on_mptr, ':')) {
				n = DbString(n);
			}
			break;
		default:
/*
			if (n->on_mptr && (n->on_type&S_TYP) >= S_MIN) {
				struct idf *id = str2idf(n->on_mptr, 0);

				sym = new_symbol();
				sym->sy_next = id->id_def;
				id->id_def = sym;
				sym->sy_class = SYMENTRY;
				sym->sy_onam = *n;
				sym->sy_idf = id;
			}
*/
			break;
		}
	}
  }
  close_scope();
  add_position_addr((char *) 0, (struct outname *) 0);
  clean_tp_tab();
  rd_close();
  return (h.oh_magic == O_CONVERTED);
}
}
