/* $Header$ */

int
sys_remove(path)
	char *path;
{
	return unlink(path) == 0;
}
