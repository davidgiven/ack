#define MININT		(1 << (sizeof(int) * 8 - 1))
#define MAXCHUNK	(-(MININT + 1))	/* Highest count we write(2).	*/

/*
 * Just write "cnt" bytes to file-descriptor "fd".
 */
wr_bytes(fd, string, cnt)
	register char	*string;
	register long	cnt;
{

	while (cnt) {
		register int n = cnt >= MAXCHUNK ? MAXCHUNK : cnt;

		if (write(fd, string, n) != n)
			wr_fatal();
		string += n;
		cnt -= n;
	}
}
