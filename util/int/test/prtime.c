/* $Id$ */

/*
	Test access to fields in struct stat
*/

#include	<sys/types.h>
#include	<sys/stat.h>
#include	<time.h>
extern char * ctime();

main(argc, argv) char *argv[];	{

	while (argc > 1)	{
		prfiltime(argv[1]);
		if (argc > 2)
			printf("\n");
		argc--;
		argv++;
	}
	exit(0);
}

prfiltime(name) char *name;	{
	struct stat buf;

	printf("%s: ", name);
	if (stat(name, &buf) != 0)
		printf(" not found\n");
	else
		prtime(&buf);
}
prtime(buf)
	struct stat *buf;
{
	printf("%lu ", buf->st_mtime);
	printf("%s\n", ctime(&buf->st_mtime));
}

