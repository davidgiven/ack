#!/bin/sh

in=$1
header=$2
source=$3

awk -f - $in >$header << "EOF"
	BEGIN {
		print "enum ir_opcode {"
	}

	/^ *[^# ]+/ {
		print "\tIR_" $2 ","
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

	/^ *[^# ]+/ {
		printf("\t{ \"%s\", ", $2)
		printf("%s", char_to_flags(substr($1, 1, 1)))
		printf(" },\n")
	}

	END {
		print "};"
	}
EOF

