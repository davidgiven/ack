int
execv(name,args)
	char *name;
	char **args;
{
	extern char **environ;
	return execve(name,args,environ);
}
