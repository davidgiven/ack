/* $Header$ */

#define MAXCREG 4

struct perm {
	struct perm *p_next;
	int p_rar[MAXCREG];
};
