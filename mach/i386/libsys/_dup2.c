int
_dup2(oldd, newd)
{
	oldd |= 64;
	return __xdup(oldd, newd);
}
