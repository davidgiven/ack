/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <stdio.h>

char	bytes_reversed = 0;
char	words_reversed = 0;
char	char_unsigned = 0;

/*
 * Determine the byte/word order in shorts/longs, assuming the size of a short
 * is 2 chars, and the size of a long is 4 chars. Not all theoretical
 * possibilities are tested; only bytes reversed and/or words reversed.
 */
determine_ordering()
{
	short	s;
	long	l;
	register char	*cp;
	register short	*sp;

	cp = (char *)&s;
	cp[0] = 0x01; cp[1] = 0x02;
	if (s != 0x01 + (0x02 << 8))
		bytes_reversed = 1;
	sp = (short *)&l;
	sp[0] = 0x0001; sp[1] = 0x0002;
	if (l != 0x0001 + (0x0002L << 16))
		words_reversed = 1;
}

/*
 * determine whether characters are unsigned or signed
 */

uns_char()
{
	char c = 0200;
	int i = c;

	if (i > 0) char_unsigned = 1;
}

main()
{
	determine_ordering();
	uns_char();
	printf("#define BYTES_REVERSED %d\n", bytes_reversed);
	printf("#define WORDS_REVERSED %d\n", words_reversed);
	printf("#define CHAR_UNSIGNED %d\n", char_unsigned);
	return 0;
}
