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
		print ""
		print "enum {"
		print "\tIRF_SIZED = 1"
		print "};"
		print ""
		print "extern const char* ir_names[IR__COUNT];"
		print "extern const char ir_flags[IR__COUNT];"
	}
EOF

awk -f - $in >$source << "EOF"
	BEGIN {
		print "#include \"ircodes.h\""
		print "const char* ir_names[IR__COUNT] = {"
	}

	/^ *[^# ]+/ {
		printf("\t\"%s\",\n", $2)
	}

	END {
		print "};"
	}
EOF

awk -f - $in >>$source << "EOF"
	BEGIN {
		print ""
		print "const char ir_flags[IR__COUNT] = {"
	}

	/^ *[^# ]+/ {
		if ($1 == "S")
			print("\tIRF_SIZED,")
		else
			print("\t0,")
	}

	END {
		print "};"
	}
EOF

