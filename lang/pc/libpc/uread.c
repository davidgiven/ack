/* $Header$ */
/* function uread(fd:integer; var b:buf; n:integer):integer; */

extern int	read();

int uread(fd,b,n) char *b; int fd,n; {
	return(read(fd,b,n));
}
