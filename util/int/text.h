/*
	Accessing the program text
*/

/* $Id$ */

#define	text_loc(a)	(*(text + (p2i(a))))

/*	The bytes in the text segment are unsigned, and this is what is
	implemented by the macros btol() and btou().  Some operands,
	however, are signed; this is indicated in the table by P or N.
	When an operand is positive, it is guaranteed that the leftmost
	bit is 0, so we can get the value by doing sign extension.  Likewise,
	when the operand is negative the leftmost bit will be 1 and again sign
	extension yields the right value.
	Actually we should test if this guarantee is indeed upheld, but that
	is just too expensive.
*/

/*	Reading the opcode.
*/
#define	nextPCbyte()	(PC+=1, btou(text_loc(PC-1)))

/*	Shortie arguments consist of the high order value, derived from
	the opcode and passed as a parameter, and the following byte.
*/
#define	S_arg(h)	(PC+=1, ((h)<<8) + btol(text_loc(PC-1)))

/*	Two-byte arguments consist of the following two bytes.
*/

#define	L_arg_2()	(PC+=2, (btol(text_loc(PC-1)) | \
				(btos(text_loc(PC-2)) << 8)))

#define	P_arg_2()	(PC+=2, (btol(text_loc(PC-1)) | \
				(btos(text_loc(PC-2)) << 8)))/* should test */

#define	N_arg_2()	(PC+=2, (btol(text_loc(PC-1)) | \
				(btos(text_loc(PC-2)) << 8)))/* should test */

#define	U_arg()		(PC+=2, (btol(text_loc(PC-1)) | \
				(btol(text_loc(PC-2)) << 8)))

/*	The L-, P-, and N-4-bytes #defines are all equal, because
	we assume our longs to be 4 bytes long.
*/

#define	L_arg_4()	(PC+=4, (btol(text_loc(PC-1)) | \
				(btol(text_loc(PC-2)) << 8) | \
				(btol(text_loc(PC-3)) << 16) | \
				(btos(text_loc(PC-4)) << 24)))

#define	P_arg_4()	(PC+=4, (btol(text_loc(PC-1)) | \
				(btol(text_loc(PC-2)) << 8) | \
				(btol(text_loc(PC-3)) << 16) | \
				(btos(text_loc(PC-4)) << 24)))/* should test */

#define	N_arg_4()	(PC+=4, (btol(text_loc(PC-1)) | \
				(btol(text_loc(PC-2)) << 8) | \
				(btol(text_loc(PC-3)) << 16) | \
				(btos(text_loc(PC-4)) << 24)))/* should test */


/*
 * #defines for argument checks.
 */

#define	arg_c(n)	((n < i_minsw || n > i_maxsw) ? \
					(wtrap(WARGC, EILLINS), 0) : n)

#define	arg_d(n)	((wsize > 2) ? (wtrap(WARGD, EILLINS), 0) : n)

#define	arg_l(n)	((n < min_off || n > max_off) ? \
					(wtrap(WARGL, EILLINS), 0) : n)

#define	arg_g(p)	((p >= HB) ? (wtrap(WARGG, EILLINS), i2p(0)) : p)

#define	arg_f(n)	((n < min_off || n > max_off) ? \
					(wtrap(WARGF, EILLINS), 0) : n)

#define	arg_n(u)	((u > i_maxuw) ? (wtrap(WARGL, EILLINS), 0) : u)

#define	arg_s(s)	((s <= 0 || s > max_off || (s & wsizem1)) ? \
				(trap(EODDZ), s) : s)

#define	arg_z(s)	((s < 0 || s > max_off || (s & wsizem1)) ? \
				(trap(EODDZ), s) : s)

#define	arg_o(s)	((s < 0 || s > max_off || ((s&wsizem1) && wsize%s)) ? \
				(trap(EODDZ), s) : s)

#define	arg_w(s)	((s <= 0 || s > max_off || (s & wsizem1)) ? \
				(trap(EODDZ), s) : s)

#define	arg_p(l)	((l >= NProc) ? (wtrap(WARGP, EILLINS), 0) : l)

#define	arg_r(n)	((n < 0 || n > 2) ? (wtrap(WARGR, EILLINS), 0) : n)

/* tests on widths */
#define	arg_wn(s)	((s != 1 && s != 2 && s != 4) ? \
				(trap(EODDZ), s) : s)

#define	arg_wf(s)	((s != 4 && s != 8) ? (trap(EODDZ), s) : s)

#define	arg_wi(s)	(((s != 2 && s != 4) || (s & wsizem1)) ? \
				(trap(EODDZ), s) : s)

/* special tests */
#define	arg_lae(p)	((p > ML) ? (trap(EBADLAE), p) : p)

#define	arg_gto(p)	((p>=HB) ? (wtrap(WGTOSTACK, EBADGTO), p) : p)

#define	arg_lin(u)	((u > NLINE) ? (trap(EBADLIN), u) : u)

/* Allocates the "text" variable that will hold the text
 * segment.
 */
void init_text(void);
/* Sets the new value of the PC register to the specified
 * value "p".
 */
void newPC(register ptr p);
