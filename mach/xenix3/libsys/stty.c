stty(fildes, arg)
	char *arg;
{
	return ioctl(fildes, ('t' << 8) | 9, arg);
}
