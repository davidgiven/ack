/* $Header$ */
/*		A S M			*/

asm_seen(s)
	char *s;
{
	/*	'asm' '(' string ')' ';'
	*/
	warning("\"asm(\"%s\")\" instruction skipped", s);
}
