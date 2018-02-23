int
_gtty(fildes,argp)
	int fildes ;
	char *argp ;
{
	return _ioctl(fildes,/*TIOCGETP*/(('t'<<8)|8),argp) ;
}
