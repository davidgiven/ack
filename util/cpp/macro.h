/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* PREPROCESSOR: DEFINITION OF MACRO DESCRIPTOR */

/*	The flags of the mc_flag field of the macro structure. Note that
	these flags can be set simultaneously.
*/
#define NOFLAG		0		/* no special flags	*/
#define	FUNC		01		/* function attached    */
#define NOREPLACE	02		/* don't replace	*/

#define	FORMALP 0200	/* mask for creating macro formal parameter	*/

/*	The macro descriptor is very simple, except the fact that the
	mc_text, which points to the replacement text, contains the
	non-ascii characters \201, \202, etc, indicating the position of a
	formal parameter in this text.
*/
struct macro	{
	struct macro *next;
	char *	mc_text;	/* the replacement text		*/
	int	mc_nps;		/* number of formal parameters	*/
	int	mc_length;	/* length of replacement text	*/
	int	mc_count;	/* # of "concurrent" invocations*/
	char	mc_flag;	/* marking this macro		*/
};


/* allocation definitions of struct macro */
extern char *st_alloc();
extern struct macro *h_macro;
#ifdef DEBUG
extern int cnt_macro;
extern char *std_alloc();
#define	new_macro() ((struct macro *) std_alloc((char **)&h_macro, sizeof(struct macro), 20, &cnt_macro))
#else
#define	new_macro() ((struct macro *) st_alloc((char **)&h_macro, sizeof(struct macro), 20))
#endif
#define	free_macro(p) st_free(p, &h_macro, sizeof(struct macro))

struct mlist {
	struct mlist *next;
	struct macro *m_mac;
	char *m_repl;
	int m_level;
};

/* allocation definitions of struct mlist */
extern char *st_alloc();
extern struct mlist *h_mlist;
#ifdef DEBUG
extern int cnt_mlist;
extern char *std_alloc();
#define	new_mlist() ((struct mlist *) std_alloc((char **)&h_mlist, sizeof(struct mlist), 20, &cnt_mlist))
#else
#define	new_mlist() ((struct mlist *) st_alloc((char **)&h_mlist, sizeof(struct mlist), 20))
#endif
#define	free_mlist(p) st_free(p, &h_mlist, sizeof(struct mlist))


/* `token' numbers of keywords of command-line processor
*/
#define	K_UNKNOWN	0
#define	K_DEFINE	1
#define	K_ELIF		2
#define	K_ELSE		3
#define	K_ENDIF		4
#define	K_IF		5
#define	K_IFDEF		6
#define	K_IFNDEF	7
#define	K_INCLUDE	8
#define	K_LINE		9
#define	K_UNDEF		10
#define K_PRAGMA	11
#define K_FILE		100	/* for dependency generator */
