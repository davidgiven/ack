/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

typedef enum {
	justlooking,mustexist,newsymbol,makeexist
} lookupstyle;

typedef enum {
	symany,symkeyw,symconst,symsconst,symprop,symreg,symtok,symset,symproc
} symtype;

typedef struct symbol {
	struct symbol *sy_next;		/* pointer to hashchain */
	char          *sy_name;		/* symbol */
	symtype        sy_type;		/* type */
	union {
	    long       syv_cstval;
	    int	       syv_stringno;
	    int	       syv_keywno;
	    int        syv_propno;
	    int	       syv_regno;
	    int	       syv_tokno;
	    int	       syv_setno;
	    int	       syv_procoff;
	} sy_value;
} symbol;

#define NSYMHASH 61
extern symbol *symhash[NSYMHASH];	/* chained hashtable */
extern symbol *lookup();
