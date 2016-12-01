#!/bin/sh
echo ""

succeeding="$(find "$@" -size 0)"
notsucceeding="$(find "$@" ! -size 0)"
skipped="$(grep -l @@SKIPPED $notsucceeding)"
timedout="$(grep -l @@TIMEDOUT $notsucceeding)"
failed="$(grep -l @@FAIL $notsucceeding)"

for a in $failed $timedout; do
    echo "**** $a"
    cat $a
    echo ""
done

echo "$(echo $succeeding | wc -w) tests passed"
echo "$(echo $notsucceeding | wc -w) tests failed to pass"
echo "$(echo $skipped | wc -w) were skipped (see build log for details)"
echo "$(echo $timedout | wc -w) timed out"
echo "$(echo $failed | wc -w) failed"
echo ""

if [ "$failed" != "" -o "$timedout" != "" ]; then
	echo "Test status: SAD FACE (tests are failing)"
	exit 1
fi
if [ "$succeeding" = "" ]; then
	echo "Test status: PUZZLED FACE (all tests were skipped)"
	exit 0
fi
if [ "$skipped" != "" ]; then
	echo "Test status: MILDLY PLEASED FACE (some tests were skipped, but the rest pass)"
	exit 0
fi
echo "Test status: HAPPY FACE (all tests are passing)"
exit 0

