execv(name,args)
	char *name;
	char **args;
{
	extern char **environ;
	execve(name,args,environ);
}
