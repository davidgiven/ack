/* $Header$ */

/*	I/O part of em_code module.
	Also contains C_open, C_close
*/
#include <system.h>
#include <alloc.h>
#include <em_path.h>
#include <em_arith.h>
#include "insert.h"

int
	C_ontmpfile = 0,
	C_sequential = 1;
Part
	*C_curr_part, *C_stable[TABSIZ];
long
	C_current_out;

#ifdef INCORE
static char	*BASE;
#endif
static File	*ofp;

#ifndef INCORE
static File	*tfr, *old_ofp;
static char	*tmpfile;
char		*strcpy(), *strcat(), *mktemp();
static char	*ibuf = 0;
#endif

static char	obuf[BUFSIZ];
static char	*opp = obuf;

static
flush() {
	if (opp != obuf && sys_write(ofp, obuf, opp - obuf) == 0) {
		C_failed();
	}
	opp = obuf;
}

#define put(c)	if (opp == &obuf[BUFSIZ]) flush(); *opp++ = (c)

C_putbyte(c)
	int c;
{
	if (C_ontmpfile) {
#ifdef INCORE
		static unsigned sz;

		if (BASE == 0) {
			BASE = Malloc(BUFSIZ);
			sz = BUFSIZ;
		}
		else if (C_current_out >= sz) {
			BASE = Srealloc(BASE, (sz <<= 1));
		}
		*(BASE + C_current_out) = c;
#endif
		C_current_out++;
#ifdef INCORE
		return;
#endif
	}
	put(c);
}

#ifndef INCORE
C_switchtotmp()
{
	if (tmpfile == 0) {
		static char tmpbuf[64];
		register char *p = tmpbuf;

		strcpy(p, TMP_DIR);
		strcat(p, "/CodeXXXXXX");
		tmpfile = mktemp(p);
		if (! sys_open(p, OP_WRITE, &old_ofp)) {
			C_failed();
		}
		if (! sys_open(p, OP_READ, &tfr)) {
			C_failed();
		}
	}
	if (! C_ontmpfile) {
		File *p = ofp;

		flush();
		ofp = old_ofp;
		old_ofp = p;
		C_ontmpfile = 1;
	}
}

C_switchtoout()
{
	if (C_ontmpfile) {
		File *p = ofp;

		flush();
		ofp = old_ofp;
		old_ofp = p;
		C_ontmpfile = 0;
	}
}
#endif

C_init(w, p)
	arith w, p;
{
}

C_open(nm)
	char *nm;
{
	/*	Open file "nm" for output
	*/

	if (nm == 0)
		ofp = STDOUT;	/* standard output	*/
	else
	if (sys_open(nm, OP_WRITE, &ofp) == 0)
		return 0;
	return 1;
}

C_close()
{
	/*	Finish the code-generation.
	*/

#ifndef INCORE
	flush();
	if (tmpfile) {
		C_switchtotmp();
		sys_close(ofp);
		ofp = old_ofp;
#else
	if (BASE) {
#endif
		if (C_curr_part) {
			C_curr_part->p_parts->pp_end = C_current_out;
		}
		if (! C_sequential) {
			outpart(0);
		}
#ifndef INCORE
		sys_close(tfr);
		sys_remove(tmpfile);
		if (ibuf) free(ibuf);
#else
		free(BASE);
#endif
	}
	flush();
	if (ofp != STDOUT)
		sys_close(ofp);
	ofp = 0;
}

C_busy()
{
	return ofp != 0; /* true if code is being generated */
}

#ifndef INCORE
static int
getbyte(b)
	long b;
{
	/*	Get the byte at offset "b" from the start of the
		temporary file, and try to do so in an efficient way.
	*/
	static long	start_core, curr_pos;

	if (b < start_core || b >= curr_pos) {
		/* the block wanted is not in core, so get it */
		long nb = (b & ~(BUFSIZ - 1));
		int n;

		flush();
		if (nb != curr_pos) {
			if (sys_seek(tfr, nb, 0, &curr_pos) == 0) {
				C_failed();
			}
		}
		if (! ibuf) {
			ibuf = Malloc(BUFSIZ);
		}
		if (sys_read(tfr, ibuf, BUFSIZ, &n) == 0) {
			C_failed();
		}
		curr_pos += n;
		start_core = nb;
	}

	return ibuf[(int) (b - start_core)];
}
#endif

C_out_parts(pp)
	register PartOfPart *pp;
{
	/*	Output the list of chunks started by "pp".
		The list is build in reverse order, so this routine is
		recursive.
	*/

	if (!pp) return;
	if (pp->pp_next) C_out_parts(pp->pp_next);

	if (pp->pp_type == INSERT) {
		outpart(pp->pp_id);
	}
	else {
		/* copy the chunk to output */
#ifdef INCORE
		register char *s = BASE + pp->pp_begin;
		char *se = BASE + pp->pp_end;

		while (s < se) {
			put(*s++);
		}
#else
		register long b = pp->pp_begin;

		while (b < pp->pp_end) {
			put(getbyte(b++));
		}
#endif
	}
}

Part *
C_findpart(part)
	int part;
{
	/*	Look for part "part" in the table.
		Return 0 if not present,
	*/
	register Part *p = C_stable[part % TABSIZ];

	while (p && p->p_id != part) {
		p = p->p_next;
	}
	return p;
}

