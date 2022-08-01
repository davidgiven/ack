extern int ioctl(int fd, unsigned int request, ...);
extern int _ioctl(int fd, int ioc, void* ptr);

int gtty(int fildes, char* argp)
{
	return ioctl(fildes,/*TIOCGETP*/(('t'<<8)|8),argp) ;
}
