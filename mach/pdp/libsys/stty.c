int
stty(fildes,argp)
	int fildes ;
	char *argp;
{
	return _ioctl(fildes,/*TIOCSETP*/(('t'<<8)|9),argp) ;
}
