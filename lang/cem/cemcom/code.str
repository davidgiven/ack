/* $Header$ */
/*	C O D E - G E N E R A T O R   D E F I N I T I O N S	*/

struct stat_block	{
	struct stat_block *next;
	label st_break;
	label st_continue;
};


/* allocation definitions of struct stat_block */
/* ALLOCDEF "stat_block" */
extern char *st_alloc();
extern struct stat_block *h_stat_block;
#define	new_stat_block() ((struct stat_block *) \
		st_alloc((char **)&h_stat_block, sizeof(struct stat_block)))
#define	free_stat_block(p) st_free(p, h_stat_block, sizeof(struct stat_block))


#define	LVAL	0
#define	RVAL	1
#define	FALSE	0
#define	TRUE	1
