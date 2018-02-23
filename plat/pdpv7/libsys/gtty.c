int
gtty(fildes,argp)
	int fildes ;
	char *argp ;
{
	return ioctl(fildes,/*TIOCGETP*/(('t'<<8)|8),argp) ;
}
