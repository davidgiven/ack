/* $Header$ */

#define _REGINFO_
typedef struct reginfo {
	char *ri_name;
	char *ri_repr;
	int   ri_size;
	int   ri_class;
	int   ri_rregvar;
	int   ri_memb[2];
} reginfo;

extern struct reginfo l_regs[];
