/* $Header$ */
/*		A S M			*/

asm_seen(s, l)
	char *s;
	int l;
{
	/*	'asm' '(' string ')' ';'
	*/
	warning("\"asm(\"%s\")\" instruction skipped", s);
}
