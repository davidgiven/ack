/* $Header$ */
/* DECLARATION SPECIFIER DEFINITION */

struct decspecs	{
	struct decspecs *next;
	struct type *ds_type;	/* single type */
	int ds_sc_given;	/* 1 if the st. class is explicitly given */
	int ds_sc;		/* storage class, given or implied */
	int ds_size;		/* LONG, SHORT or 0 */
	int ds_unsigned;	/* 0 or 1 */
};


/* allocation definitions of struct decspecs */
/* ALLOCDEF "decspecs" */
extern char *st_alloc();
extern struct decspecs *h_decspecs;
#define	new_decspecs() ((struct decspecs *) \
		st_alloc((char **)&h_decspecs, sizeof(struct decspecs)))
#define	free_decspecs(p) st_free(p, h_decspecs, sizeof(struct decspecs))


extern struct decspecs null_decspecs;
