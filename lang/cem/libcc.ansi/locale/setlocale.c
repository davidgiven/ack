/*
 * setlocale - set the programs locale
 */
/* $Header$ */

#include	<locale.h>

struct lconv _lc;

char *
setlocale(int category, const char *locale)
{
	if (strcmp(locale, "C")) return (char *)NULL;
	
	switch(category) {
	case LC_ALL:
	case LC_COLLATE:
	case LC_CTYPE:
	case LC_MONETARY:
	case LC_NUMERIC:
	case LC_TIME:
		return locale;
	default:
		return (char *)NULL;
	}
}
