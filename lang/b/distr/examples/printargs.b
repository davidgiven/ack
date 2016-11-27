main() {
	extrn argv;
	auto i;

	i = 1;
	printf("%d args:*n", argv[0]);
	while (i <= argv[0])
		printf("%s*n", argv[i++]);
	return(0);
}
