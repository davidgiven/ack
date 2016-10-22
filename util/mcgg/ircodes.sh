#!/bin/sh

in=$1
header=$2
source=$3

awk -f - $in >$header << "EOF"
	BEGIN {
		print "enum ir_opcode {"
	}

	/^ *[^# ]+/ {
		print "\tIR_" $3 ","
	}

	END {
		print "\tIR__COUNT"
		print "};"
	}
EOF

awk -f - $in >$source << "EOF"
	BEGIN {
		print "#include \"ircodes.h\""
		print "const struct ir_data ir_data[IR__COUNT] = {"
	}

	function char_to_flags(c) {
		if (c == "S") return "IRF_SIZED"
		return "0"
	}

	function char_to_type(c) {
		if (c ~ /[A-Za-z]/) return "'"c"'"
		if (c == "?")       return "'?'"
		if (c == ".")       return "0"
	}

	/^ *[^# ]+/ {
		printf("\t{ \"%s\", ", $3)
		printf("%s, ", char_to_flags(substr($1, 1, 1)))
		printf("%s, ", char_to_type(substr($2, 1, 1)))
		printf("%s, ", char_to_type(substr($2, 3, 1)))
		printf("%s, ", char_to_type(substr($2, 4, 1)))
		printf(" },\n")
	}

	END {
		print "};"
	}
EOF

