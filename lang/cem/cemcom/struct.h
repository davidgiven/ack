/* $Header$ */
/* SELECTOR DESCRIPTOR */

struct sdef	{		/* for selectors */
	struct sdef *next;
	int sd_level;
	struct idf *sd_idf;	/* its name */
	struct sdef *sd_sdef;	/* the next selector */
	struct type *sd_stype;	/* the struct it belongs to */
	struct type *sd_type;	/* its type */
	arith sd_offset;
};

extern char *st_alloc();


/* allocation definitions of struct sdef */
/* ALLOCDEF "sdef" */
extern char *st_alloc();
extern struct sdef *h_sdef;
#define	new_sdef() ((struct sdef *) \
		st_alloc((char **)&h_sdef, sizeof(struct sdef)))
#define	free_sdef(p) st_free(p, h_sdef, sizeof(struct sdef))


struct tag	{		/* for struct-, union- and enum tags */
	struct tag *next;
	int tg_level;
	int tg_busy;	/* non-zero during declaration of struct/union pack */
	struct type *tg_type;
};



/* allocation definitions of struct tag */
/* ALLOCDEF "tag" */
extern char *st_alloc();
extern struct tag *h_tag;
#define	new_tag() ((struct tag *) \
		st_alloc((char **)&h_tag, sizeof(struct tag)))
#define	free_tag(p) st_free(p, h_tag, sizeof(struct tag))


struct sdef *idf2sdef();
