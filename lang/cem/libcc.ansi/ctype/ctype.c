/*
 * ctype.c - character handling
 */
/* $Header$ */

int	isalnum(int c);		/* Alpha numeric character */
int	isalpha(int c);		/* Alpha character */
int 	iscntrl(int c);		/* Control character */
int	isdigit(int c);		/* Digit character */
int 	isgraph(int c);		/* Graphical character */
int	islower(int c);		/* Lower case character */
int	isprint(int c);		/* Printable character */
int	ispunct(int c);		/* Punctuaction character */
int	isspace(int c);		/* Space character */
int	isupper(int c);		/* Upper case character */
int	isxdigit(int c);	/* Hexdecimal digit character */

int	tolower(int c);		/* Convert to lower case character */
int	toupper(int c);		/* Convert to upper case character */

int	isalnum(int c) {		/* Alpha numeric character */
	 return	 isdigit(c) || isalpha(c);
}

int	isalpha(int c) {		/* Alpha character */
	 return	 isupper(c) || islower(c);
}

int 	iscntrl(int c) {		/* Control character */
	 return	 (c >= 0 && c <= 0x1f) || c == 0x7f;
}

int	isdigit(int c) {		/* Digit character */
	 return	 (unsigned)(c - '0') < 10;
}

int 	isgraph(int c) {		/* Graphical character */
	 return	 isprint(c) && c != ' ';
}

int	islower(int c) {		/* Lower case character */
	 return	 (unsigned)(c - 'a') < 26;
}

int	isprint(int c) {		/* Printable character */
	 return	 c > ' ' && c < 0x7f;
}

int	ispunct(int c) {		/* Punctuation character */
	 return	 isprint(c) && !(c == ' ' || isalnum(c));
}

int	isspace(int c) {		/* Space character */
	 return	 c == ' ' || c == '\f' || c == '\n' ||
		c == '\r' || c == '\t' || c == '\v';
}

int	isupper(int c) {		/* Upper case character */
	return	(unsigned)(c - 'A') < 26;
}

int	isxdigit(int c) {		/* Hexdecimal digit character */
	return	isdigit(c) ||
		(c >= 'A' && c <= 'F') ||
		(c >= 'a' && c <= 'f');
}

int	tolower(int c) {		/* Convert to lower case character */
	if (!isupper(c)) return c;
	else return c - 'A' + 'a';
}

int	toupper(int c) {		/* Convert to upper case character */
	if (!islower(c)) return c;
	else return c - 'a' + 'A';
}
