/*
 * ctype.c - character handling
 */
/* $Header$ */

int	isalnum(c) {		/* Alpha numeric character */
	 return	 isdigit(c) || isalpha(c);
}

int	isalpha(c) {		/* Alpha character */
	 return	 isupper(c) || islower(c);
}

int 	iscntrl(c) {		/* Control character */
	 return	 (c >= 0 && c <= 0x1f) || c == 0x7f;
}

int	isdigit(c) {		/* Digit character */
	 return	 (unsigned)(c - '0') < 10;
}

int 	isgraph(c) {		/* Graphical character */
	 return	 isprint(c) && c != ' ';
}

int	islower(c) {		/* Lower case character */
	 return	 (unsigned)(c - 'a') < 26;
}

int	isprint(c) {		/* Printable character */
	 return	 c > ' ' && c < 0x7f;
}

int	ispunct(c) {		/* Punctuation character */
	 return	 isprint(c) && !(c == ' ' || isalnum(c));
}

int	isspace(c) {		/* Space character */
	 return	 c == ' ' || c == '\f' || c == '\n' ||
		c == '\r' || c == '\t' || c == '\v';
}

int	isupper(c) {		/* Upper case character */
	return	(unsigned)(c - 'A') < 26;
}

int	isxdigit(c) {		/* Hexdecimal digit character */
	return	isdigit(c) ||
		(c >= 'A' && c <= 'F') ||
		(c >= 'a' && c <= 'f');
}

int	tolower(c) {		/* Convert to lower case character */
	if (!isupper(c)) return c;
	else return c - 'A' + 'a';
}

int	toupper(c) {		/* Convert to upper case character */
	if (!islower(c)) return c;
	else return c - 'a' + 'A';
}
