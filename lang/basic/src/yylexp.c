/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

int toknum;

yylexp() 
{
/* als toknum != 0 dan bevat toknum een door LLmessage back-ge-pushed token */

int t;

	if ( toknum == 0 ) 
		return(yylex());
	else {
		t = toknum;
		toknum = 0;
		return(t);
	}
}

