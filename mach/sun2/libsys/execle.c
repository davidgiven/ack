execle(name,args)
	char *name;
	char *args;
{
	char **p = &args;
	while (*p++) ;

	execve(name,&args,*p);
}
