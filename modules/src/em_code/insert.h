/* Structures used for the C_insertpart, C_beginpart, and C_endpart
   mechanism. Each part consists of a list of chunks. Each chunk is
   either another part, or a piece of text limited by a begin- and an
   end-pointer.
*/

/* $Id$ */

#include <system.h>
#include <local.h>

#if BIGMACHINE
#define INCORE	/* mechanism implemented incore */
#endif

typedef struct partofpart {
	struct partofpart	*pp_next;
	char			pp_type;
#define TEXT	0
#define INSERT	1
	union {
		struct {
			long	ppu_begin, ppu_end;
		} ppu_s;
		int	ppu_id;
	} pp_u;
#define pp_begin	pp_u.ppu_s.ppu_begin
#define pp_end		pp_u.ppu_s.ppu_end
#define pp_id		pp_u.ppu_id
} PartOfPart;

typedef struct part {
	struct part	*p_next;	/* next part in hash chain */
	char		p_flags;
#define BUSY	1
	PartOfPart	*p_parts;	/* chunks of this part */
	struct part	*p_prevpart;	/* implements stack of active parts */
	int		p_id;		/* id of this part */
} Part;

#define TABSIZ	32

extern int
	C_ontmpfile, C_sequential;
extern Part
	*C_curr_part;
#ifdef INCORE
extern char
	*C_current_out, *C_BASE;
#define C_opp C_current_out
#else
extern long
	C_current_out;
extern char	*C_opp;
#define C_BASE 0
#endif
extern int (*C_outpart)(), (*C_swtout)(), (*C_swttmp)();

extern File	*C_ofp;

#ifndef INCORE
extern File	*C_tfr;
extern char	*C_tmpfile;
#endif

extern char	*C_top;
extern char	*C_ibuf;

#define put(c)	if (C_opp == C_top) C_flush(); *C_opp++ = (c)

