/* $Id$ */

/*
	Test access to args and environ
*/

extern char **environ;

main(argc, argv, envp)
	char **argv, **envp;
{
	int i;

	printf("# of args: %d\n", argc);

	printf("\n");
	for (i = 0; argv[i] != 0; i++)
		printf("$%d=%s\n", i, argv[i]);

	printf("\n");
	for (i = 0; envp[i] != 0; i++)
		printf("%s\n", envp[i]);

	if (envp != environ)	{
		printf("\n");
		printf("different environment from `environ':\n");
		for (i = 0; envp[i] != 0; i++)
			printf("%s\n", envp[i]);
		printf("\n");
	}
	exit(0);
}
