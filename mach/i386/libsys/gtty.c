gtty(fildes, arg)
	char *arg;
{
	return ioctl(fildes, ('t' << 8) | 8, arg);
}
