/* $Header$ */
/* function uwrite(fd:integer; var b:buf; n:integer):integer; */

extern int	write();

int uwrite(fd,b,n) char *b; int fd,n; {
	return(write(fd,b,n));
}
