/* $Id$ */

/*	The Return Status Block contains, in push order:
	FIL, LIN, LB, PC, PI, rsbcode

	In a trap this is preceeded by:
	FRA, FRASize, FRA_def, trap_nr
*/

/* offsets to be added to a local base */
extern int rsb_rsbcode;
extern int rsb_PI;
extern int rsb_PC;
extern int rsb_LB;
extern int rsb_LIN;
extern int rsb_FIL;
extern int rsbsize;

/*	The last item stored in the Return Status Block is a word containing
	a code describing the type of the RSB.
*/

#define	RSBMASK		0xfff0
#define	RSBCODE		0x2b90		/* 0rrr rrrr rrrr 0000, r = random */
#define	RSB_STP		(RSBCODE + 1)	/* in first RSB */
#define	RSB_CAL		(RSBCODE + 2)	/* in RSB from call */
#define	RSB_RTT		(RSBCODE + 3)	/* in RSB from returnable trap */
#define	RSB_NRT		(RSBCODE + 4)	/* in RSB from non-returnable trap */

#define	is_LB(p)	((st_lds(p+rsb_rsbcode, wsize) & RSBMASK) == RSBCODE)

void init_rsb(void);
void pushrsb(int rsbcode);
int poprsb(int rtt);
