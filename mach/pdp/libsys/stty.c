int stty(fildes,argp)
	int fildes ;
	char *argp;
{
	return ioctl(fildes,/*TIOCSETP*/(('t'<<8)|9),argp) ;
}
