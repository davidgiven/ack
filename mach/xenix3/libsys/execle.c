int
execle(name,args)
	char *name;
	char *args;
{
	char **p = &args;
	while (*p++) ;

	return execve(name,&args,*p);
}
