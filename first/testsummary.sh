#!/bin/sh
notsucceeding=$(find "$@" ! -size 0)
echo ""
echo "$(echo $notsucceeding | wc -w) tests failed to pass"

skipped=$(grep -l @@SKIPPED $notsucceeding)
echo "$(echo $skipped | wc -w) were skipped (see build log for details)"

timedout=$(grep -l @@TIMEDOUT $notsucceeding)
echo "$(echo $timedout | wc -w) timed out"

failed=$(grep -l @@FAIL $notsucceeding)
echo "$(echo $failed | wc -w) failed"

echo ""
for a in $failed $timedout; do
    echo "**** $a"
    cat $a
    echo ""
done
exec test "$failed" == "" -o "$timedout" == ""
