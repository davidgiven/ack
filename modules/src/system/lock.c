/* $Header$ */

int
sys_lock(path)
	char *path;
{
	char buf[1024];
	char *tmpf = ".lockXXXXXX";
	char *strrindex();
	char *p;
	int ok, fd;

	strcpy(buf, path);
	if (p = strrindex(buf, '/')) {
		++p;
		strcpy(p, tmpf);
	}
	else
		strcpy(buf, tmpf);
	mktemp(buf);
	if ((fd = creat(buf, 0)) < 0)
		return 0;
	close(fd);
	ok = (link(buf, path) == 0);
	unlink(buf);
	return ok;
}
