_gtty(fildes, arg)
	char *arg;
{
	return _ioctl(fildes, ('t' << 8) | 8, arg);
}
