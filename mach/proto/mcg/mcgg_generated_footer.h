static void dumpCover(NODEPTR_TYPE p, int goalnt, int indent) {
#ifdef TRACE
	int eruleno = burm_rule(STATE_LABEL(p), goalnt);
	const short *nts = burm_nts[eruleno];
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

/* vim: set sw=4 ts=4 expandtab : */
