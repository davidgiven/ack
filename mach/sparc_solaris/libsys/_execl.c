_execl(name,args)
	char *name;
	int args;
{
	extern char **environ;

	_execve(name,&args,environ);
}
