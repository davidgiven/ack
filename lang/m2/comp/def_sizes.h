/* D E F A U L T   S I Z E S   A N D   A L I G N M E N T S */

/* $Header$ */

#define MAXSIZE		8	/* the maximum of the SZ_* constants	*/
/* target machine sizes	*/
#define	SZ_CHAR		(arith)1
#define SZ_WORD		(arith)4
#define	SZ_INT		(arith)4
#define	SZ_LONG		(arith)4
#define	SZ_FLOAT	(arith)4
#define	SZ_DOUBLE	(arith)8
#define	SZ_POINTER	(arith)4
/* target machine alignment requirements	*/
#define	AL_CHAR		1
#define AL_WORD		(int) SZ_WORD
#define	AL_INT		(int) SZ_WORD
#define	AL_LONG		(int) SZ_WORD
#define	AL_FLOAT	(int) SZ_WORD
#define	AL_DOUBLE	(int) SZ_WORD
#define	AL_POINTER	(int) SZ_WORD
#define AL_STRUCT	1
