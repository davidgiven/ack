/* $Id$ */
#ifndef __USG
static struct zonetable {
	int	offset;
	char	*stdzone;
	char	*dstzone;
} zonetable[] = {
	-3*60-30, "NST", 0,	/* new foundland ??? */
	-4*60, "AST", "ADT",	/* atlanic */
	-5*60, "EST", "EDT",	/* eastern */
	-6*60, "CST", "CDT",	/* central */
	-7*60, "MST", "MDT",	/* mountain */
	-8*60, "PST", "PDT",	/* pacific */
	-9*60, "YST", "YDT",	/* yukon */
	-10*60, "AST", "ADT",	/* aleutian */
	-10*60, "HST", "HDT",	/* hawai, never found */
	0, "GMT", 0,		/* Greenwich */
	0, "WET", "WDT",	/* west european, never found */
	1*60,"MET", "MDT",	/* middle european */
	2*60, "EET", "EDT",	/* east european */
	8*60, "WST", 0,	/* west australia */
	9*60, "JST", 0,	/* japan */
	9*60+30, "CST", 0,	/* also australia ??? */
	10*60, "EST", 0,	/* also australia ??? */
	-1			
};

char *
timezone(zone, dst)
{
	register struct zonetable *p = zonetable;
	static char buf[16];
	register char *c;
	int i;

	while (p->offset != -1) {
		if (zone == -p->offset) {
			if (dst && p->dstzone) return p->dstzone;
			if (!dst && p->stdzone) return p->stdzone;
		}
		p++;
	}
	*c++ = 'G';
	*c++ = 'M';
	*c++ = 'T';
	if (zone < 0) {
		zone = - zone;
		*c++ = '+';
	}
	else	*c++ = '-';
	i = (zone / 60) % 24;
	if (i >= 10) {
		*c++ = i / 10 + '0';
	}
	*c++ = i % 10 + '0';
	i = zone % 60;
	*c++ = i / 10 + '0';
	*c++ = i % 10 + '0';
	*c = '\0';
	return buf;
}
#endif
