#!/bin/sh

in=$1
header=$2
source=$3

awk -f - $in >$header << "EOF"
	BEGIN {
		print "enum {"
	}

	/^[^#]+/ {
		print "\tIR_" $1 ","
	}

	END {
		print "};"
	}
EOF

awk -f - $in >$source << "EOF"
	BEGIN {
		print "#include \"mcg.h\""
		print "#include \"ir.h\""
		print "const char* ir_names[] = {"
	}

	/^[^#]+/ {
		printf("\t\"%s\",\n", $1)
	}

	END {
		print "};"
	}
EOF

