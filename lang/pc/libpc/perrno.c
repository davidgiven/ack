/* $Header$ */
/* function perrno:integer; extern; */

extern int	errno;

int perrno() {
	return(errno);
}
