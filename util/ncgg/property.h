/* $Header$ */

typedef struct propinfo {
	int	pr_size;
	short	pr_regset[SZOFSET(MAXREGS)];
} propinfo;

extern struct propinfo l_props[];
