#define RCSID0 "$Header$"

/*
 * Zilog Z80 options
 */
#define	THREE_PASS		/* distinguish short and long branches */
#define	LISTING			/* enable listing facilities */
#define RELOCATION

#undef ISALNUM
#define ISALNUM(c)	(isalnum(c) || c == '_' || c == '.')
