/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	token.h		*/

extern struct token {
	long	t_lval;
	char	*t_sval;
} token;

extern ind;
void repeat_token();
char *tokenname();
int tabulated();
