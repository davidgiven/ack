/* $Header$ */

struct regvar {
	struct regvar  *rv_next;
	long		rv_off;
	int		rv_size;
	int		rv_type;
	int		rv_score;
	int		rv_reg;
};

struct regassigned {
	struct regvar  *ra_rv;
	int		ra_score;
};

extern struct regvar *rvlist;
extern int nregvar[];
extern struct regassigned *regassigned[];
