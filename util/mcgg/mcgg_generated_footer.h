static void dumpCover(NODEPTR_TYPE p, int goalnt, int indent) {
#ifdef TRACE
	int eruleno = burm_rule(STATE_LABEL(p), goalnt);
	short *nts = burm_nts[eruleno];
	NODEPTR_TYPE kids[10];
	int i;

	for (i = 0; i < indent; i++)
		fprintf(stderr, " ");
	fprintf(stderr, "%s\n", burm_string[eruleno]);
	burm_kids(p, eruleno, kids);
	for (i = 0; nts[i]; i++)
	{
		if (kids[i])
			dumpCover(kids[i], nts[i], indent + 1);
		else
			fprintf(stderr, "failed!\n");
	}
#endif
}

static NODEPTR_TYPE tree(int op, NODEPTR_TYPE l, NODEPTR_TYPE r) {
	NODEPTR_TYPE p = malloc(sizeof *p);

	assert(p);
	p->op = op;
	p->kids[0] = l; p->kids[1] = r;
	return p;
}

int main(void) {
	NODEPTR_TYPE p;

	p = tree(STORE,
			tree(LABEL, 0, 0),
			tree(ADD,
				tree(LOAD,
					tree(LABEL, 0, 0),
					0
				),
				tree(CONST, 0, 0)
			)
		);
	burm_label(p);
	dumpCover(p, 1, 0);
	return 0;
}

