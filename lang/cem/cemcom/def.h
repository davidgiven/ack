/* $Header$ */
/* IDENTIFIER DEFINITION DESCRIPTOR */

struct def	{		/* for ordinary tags */
	struct def *next;
	int df_level;
	struct type *df_type;
	int df_sc;		/*	may be:
					GLOBAL, STATIC, EXTERN, IMPLICIT,
					TYPEDEF,
					FORMAL, AUTO,
					ENUM, LABEL
				*/
	int df_register;	/* REG_NONE, REG_DEFAULT or REG_BONUS	*/
	char df_initialized;	/* an initialization has been generated */
	char df_alloc;		/* 0, ALLOC_SEEN or ALLOC_DONE */
	char df_used;		/* set if idf is used */
	char df_formal_array;	/* to warn if sizeof is taken */
	arith df_address;
};

#define	ALLOC_SEEN	1	/* an allocating declaration has been seen */
#define	ALLOC_DONE	2	/* the allocating declaration has been done */

#define REG_NONE	0	/* no register candidate */
#define REG_DEFAULT	1	/* register candidate, not declared as such */
#define REG_BONUS	10	/* register candidate, declared as such */


/* allocation definitions of struct def */
/* ALLOCDEF "def" */
extern char *st_alloc();
extern struct def *h_def;
#define	new_def() ((struct def *) \
		st_alloc((char **)&h_def, sizeof(struct def)))
#define	free_def(p) st_free(p, h_def, sizeof(struct def))

