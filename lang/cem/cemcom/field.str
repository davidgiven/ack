/* $Header$ */
/* FIELD DESCRIPTOR */

struct field	{	/* for field specifiers	*/
	struct field *next;
	arith fd_mask;
	int fd_shift;
	int fd_width;
	struct sdef *fd_sdef;	/* upward pointer	*/
};


/* allocation definitions of struct field */
/* ALLOCDEF "field" */
extern char *st_alloc();
extern struct field *h_field;
#define	new_field() ((struct field *) \
		st_alloc((char **)&h_field, sizeof(struct field)))
#define	free_field(p) st_free(p, h_field, sizeof(struct field))

