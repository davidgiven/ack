extern int _ioctl(int fd, int ioc, void* ptr);

int stty(int fildes, char* argp)
{
	return _ioctl(fildes,/*TIOCSETP*/(('t'<<8)|9),argp) ;
}
