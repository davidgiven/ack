/* $Header$ */

int
sys_unlock(path)
	char *path;
{
	return unlink(path) == 0;
}
