/* $Header$ */

int
sys_chmode(path, mode)
	char *path;
	int mode;
{
	return chmod(path, mode) == 0;
}
