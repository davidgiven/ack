/* $Id$ */
int
strcspn(string, notin)
	char *string;
	char *notin;
{
	register char *s1, *s2;

	for (s1 = string; *s1; s1++) {
		for(s2 = notin; *s2 != *s1 && *s2; s2++) /* nothing */ ;
		if (*s2) break;
	}
	return s1 - string;
}
