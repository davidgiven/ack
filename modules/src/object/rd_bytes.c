#define MININT		(1 << (sizeof(int) * 8 - 1))
#define MAXCHUNK	(-(MININT + 1))	/* Highest count we write(2).	*/

/*
 * We don't have to worry about byte order here.
 * Just read "cnt" bytes from file-descriptor "fd".
 */
int 
rd_bytes(fd, string, cnt)
	register char	*string;
	register long	cnt;
{

	while (cnt) {
		register int n = cnt >= MAXCHUNK ? MAXCHUNK : cnt;

		if (read(fd, string, n) != n)
			rd_fatal();
		string += n;
		cnt -= n;
	}
}
