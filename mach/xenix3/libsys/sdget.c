char *
sdget(path, flags, size, mode)
	char *path;
	long size;
{
	if ((flags  & 2) == 2) {
		if (size > 0) {
			extern char *_sdget();

			if (size >= 0x10000L) {
				extern int errno;

				errno = 22;
				return (char *) -1;
			}
		}
	}
	return _sdget(path, flags, (unsigned) size - 1, mode);
}
