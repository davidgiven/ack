int
execl(name,args)
	char *name;
	int args;
{
	extern char **environ;

	return execve(name,&args,environ);
}
