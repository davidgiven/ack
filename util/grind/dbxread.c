/* $Header$
   Read the symbol table from an ACK a.out format file.
*/

#include <stb.h>
#include <alloc.h>
#include <assert.h>

#include "position.h"
#include "file.h"
#include "symbol.h"
#include "idf.h"
#include "scope.h"
#include "rd.h"

extern char		*strindex();
extern struct outname	*DbxString();

int			saw_code = 0;

static char		*AckStrings;	/* ACK a.out string table */
static struct outname	*AckNames;	/* ACK a.out symbol table entries */
static unsigned int	NAckNames;	/* Number of ACK symbol table entries */
static struct outname	*EndAckNames;	/* &AckNames[NAckNames] */

/* Read the symbol table from file 'f', which is supposed to be an
   ACK a.out format file. Offer DBX strings to the DBX string parser.
*/
int
DbxRead(f)
  char	*f;
{
  struct outhead h;
  register struct outname *n;
  register struct outname *line_file = 0;
  long OffsetStrings;
  int had_lbrac = 0;

  /* Open file, read header, and check magic word */
  if (! rd_open(f)) {
  	fatal("%s: not an ACK object file", f);
  }
  rd_ohead(&h);
  if (BADMAGIC(h) && h.oh_magic != O_CONVERTED) {
  	fatal("%s: not an ACK object file", f);
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

  /* Offer strings to the DBX string parser if they contain a ':'.
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
			while (CurrentScope != PervasiveScope) {
				close_scope();
			}
			saw_code = 0;
			sym = add_file(n->on_mptr);

			if (! listfile) newfile(sym->sy_idf);
			open_scope(sym, 0);
			sym->sy_file->f_scope = CurrentScope;
			FileScope = CurrentScope;
			clean_tp_tab();
			/* fall through */
		case N_SOL:
			if (! line_file) line_file = n;
			else line_file->on_mptr = n->on_mptr;
			break;
		case N_MAIN:
			newfile(FileScope->sc_definedby->sy_idf);
			break;
		case N_SLINE:
			assert(line_file);
			if (! saw_code && !CurrentScope->sc_bp_opp) {
			    CurrentScope->sc_bp_opp = n->on_valu;
			    if (! CurrentScope->sc_start) {
				CurrentScope->sc_start = n->on_valu;
				if (CurrentScope->sc_has_activation_record) {
					add_scope_addr(CurrentScope);
				}
			    }
			}
			saw_code = 1;
			add_position_addr(line_file->on_mptr, n);
			break;
		case N_LBRAC:	/* block, desc = nesting level */
			if (had_lbrac) {
				open_scope((p_symbol) 0, 0);
				saw_code = 0;
			}
			else {
				register p_scope sc = 
					get_scope_from_addr(n->on_valu);

				if (!sc || sc->sc_bp_opp) {
					had_lbrac = 1;
				}
				else CurrentScope = sc;
			}
			break;
#ifdef N_SCOPE
		case N_SCOPE:
			if (n->on_mptr && strindex(n->on_mptr, ':')) {
				n = DbxString(n);
			}
			break;
#endif
		case N_RBRAC:	/* end block, desc = nesting level */
			had_lbrac = 0;
			if (CurrentScope != FileScope) close_scope();
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
			if (had_lbrac) {
				open_scope((p_symbol) 0, 0);
				saw_code = 0;
				had_lbrac = 0;
			}
			if (n->on_mptr && strindex(n->on_mptr, ':')) {
				n = DbxString(n);
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
  rd_close();
  return (h.oh_magic == O_CONVERTED);
}
