/* $Id$ */

/* Symbol table data structure.
   Each identifier structure refers to a list of possible meanings of this
   identifier. Each of these meanings is represented by a "symbol" structure.
*/

typedef union constant {	/* depends on type */
  long	co_ival;
  double co_rval;
  char *co_sval;
  char *co_setval;
} t_const, *p_const;

typedef struct name {
  long	nm_value;		/* address or offset */
  struct scope *nm_scope;	/* for names that define a scope */
} t_name, *p_name;

typedef struct symbol {
  struct symbol	*sy_next;	/* link to next meaning */
  struct symbol	*sy_prev_sc;	/* link to previous decl in scope */
  struct type	*sy_type;	/* type of symbol */
  int		sy_class;
#define CONST		0x0001
#define TYPE		0x0002
#define TAG		0x0004
#define MODULE		0x0008
#define PROC		0x0010
#define FUNCTION	0x0020
#define VAR		0x0040
#define REGVAR		0x0080
#define LOCVAR		0x0100
#define VARPAR		0x0200
#define FIELD		0x0400
#define FILESYM		0x0800	/* a filename */
#define FILELINK	0x1000	/* a filename without its suffix */
#define LBOUND		0x2000	/* lower bound of array descriptor */
#define UBOUND		0x4000	/* upper bound of array descriptor */
  struct idf	*sy_idf;	/* reference back to its idf structure */
  struct scope	*sy_scope;	/* scope in which this symbol resides */
  union {
	t_const	syv_const;	/* CONST */
	t_name	syv_name;
	struct file *syv_file;		/* for FILESYM */
	struct symbol *syv_fllink;	/* for FILELINK */
	struct symbol *syv_descr;	/* for LBOUND and UBOUND */
	struct fields *syv_field;
  }	sy_v;
#define sy_const	sy_v.syv_const
#define sy_name		sy_v.syv_name
#define sy_file		sy_v.syv_file
#define sy_filelink	sy_v.syv_fllink
#define sy_field	sy_v.syv_field
#define sy_descr	sy_v.syv_descr
} t_symbol, *p_symbol;

/* ALLOCDEF "symbol" 50 */

extern p_symbol	NewSymbol(), Lookup(), Lookfromscope(), add_file();
extern p_symbol identify();

extern p_symbol	currfile, listfile;
