execl(name,args)
	char *name;
	int args;
{
	extern char **environ;

	execve(name,&args,environ);
}
