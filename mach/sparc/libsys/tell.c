long
tell(f)
{
	long lseek();

	return lseek(f, 0L, 1);
}
