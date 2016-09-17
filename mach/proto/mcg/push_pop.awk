BEGIN {
	print "#include <stdbool.h>"
	print "#include \"push_pop.h\""
	print ""

	s = 0;
	count = 0;
}

/^aar/ {
	s = 1;
}

/^[a-z]/ {
	if (!s)
		next;

	opcode[count++] = $1;
	data[$1] = $3;
}

END {
	for (op in data)
	{
		print "static const struct stackop so_" op "[] = {";

		pushpops = data[op]
		if (pushpops != "0")
		{
			for (i=1; i<=length(pushpops); i+=2)
			{
				printf "\t{ ";
				if (substr(pushpops, i, 1) == "+")
					printf "true, ";
				else
					printf "false, ";

				printf("'%s' },\n", substr(pushpops, i+1, 1));
			}
		}
		print "\t{ false, 0 }"

		print "};";
		print "";
	}

	print "const struct stackop* stackops[] = {";
	for (i=0; i<count; i++)
		print "\t&so_" opcode[i] ","
	print "};"
}

