int
_execl(name,args)
	char *name;
	int args;
{
	extern char **environ;

	return _execve(name,&args,environ);
}
