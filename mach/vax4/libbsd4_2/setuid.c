setuid(u)
{
	return setreuid(u,u);
}
