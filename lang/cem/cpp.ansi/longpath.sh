#!/bin/sh
# Long file names in the preprocessor: the line directives and __FILE__ must
# carry the whole name (they were formatted into fixed buffers of 256 and
# 1024 bytes, and a longer name overran them).
#
#	longpath.sh <cpp.ansi>

abs() { case $1 in /*) echo "$1" ;; *) echo "$PWD/$1" ;; esac; }

cpp=$(abs "$1")

work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
cd "$work" || exit 1
failed=0

fail() { echo "@@FAIL cpp.ansi: $*"; failed=1; }

# A directory more than 600 characters deep (12 levels of 50).
long=$work
i=0
while [ $i -lt 12 ]; do
	long=$long/dddddddddddddddddddddddddddddddddddddddddddddddddd
	i=$((i + 1))
done
mkdir -p "$long" || exit 1
printf 'char *f = __FILE__;\n#include "h.h"\nint y;\n' > "$long/t.c"
echo 'int z;' > "$long/h.h"

"$cpp" "$long/t.c" > out1 2> err1
status=$?
if [ $status -eq 0 ]; then
	grep -q "^int z;" out1 || fail "long path: included file missing"
	grep "^char \*f = " out1 | grep -F -q "\"$long/t.c\"" || fail "long path: __FILE__ is not the full name"
	grep -F -q "\"$long/h.h\"" out1 || fail "long path: no line directive for h.h"
	# every line directive names t.c or h.h in full
	if grep '^# [0-9]' out1 | grep -F -v -q "\"$long/"; then
		fail "long path: a line directive does not carry the full name"
	fi
else
	cat err1
	fail "long path: exit status $status"
fi

# A 2000-character name from #line.
name=$(awk 'BEGIN { for (i = 0; i < 2000; i++) printf "n" }')
printf 'int x;\n#line 10 "%s"\nchar *f = __FILE__;\nint y;\n' "$name" > "$long/l.c"

"$cpp" "$long/l.c" > out2 2> err2
status=$?
if [ $status -eq 0 ]; then
	grep -F -q "# 10 \"$name\"" out2 || fail "#line: no directive for line 10"
	grep "^char \*f = " out2 | grep -F -q "\"$name\"" || fail "#line: __FILE__ is not the full name"
else
	cat err2
	fail "#line: exit status $status"
fi

exit $failed
