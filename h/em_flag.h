/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* flags */
#define	EM_PAR	0017	/* parameter type */
#define	EM_FLO	0060	/* flow information */

/* types */
#define PAR_NO	0000	/* no parameter */
#define PAR_C	0001	/* constant */
#define PAR_D	0002	/* double word constant */
#define PAR_N	0003	/* numeric (>=0) */
#define PAR_F	0004	/* address offset */
#define PAR_L	0005	/* addressing locals/parameters */
#define PAR_G	0006	/* addressing globals */
#define	PAR_W	0007	/* size: word multiple, fits word, possibly indirect */
#define PAR_S	0010	/* size: word multiple */
#define PAR_Z	0011	/* size: zero or word multiple */
#define PAR_O	0012	/* size: word multiple or word fraction */
#define PAR_P	0013	/* procedure name */
#define PAR_B	0014	/* branch: instruction label */
#define PAR_R	0015	/* register number (0,1,2) */

/* flow */
#define	FLO_NO	0000	/* straight on */
#define	FLO_C	0020	/* conditional branch */
#define	FLO_P	0040	/* procedure: call and return */
#define	FLO_T	0060	/* terminate: no return */
