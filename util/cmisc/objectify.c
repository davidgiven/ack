#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, const char* argv[])
{
	size_t count = 0;

	if (argc != 2)
	{
		fprintf(stderr, "syntax: objectify <symbol>\n");
		exit(1);
	}

	printf("#include <stdint.h>\n");
	printf("#include <unistd.h>\n");
	printf("const uint8_t %s_data[] = {", argv[1]);
	for (;;)
	{
		int c = getchar();
		if (c == -1)
			break;
		if ((count & 15) == 0)
			putchar('\n');
		printf("0x%02x, ", c);
		count++;
	}
	printf("\n};\n");
	printf("const size_t %s_len = %zu;\n", argv[1], count);

	return 0;
}

