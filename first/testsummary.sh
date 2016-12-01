#!/bin/sh
failed=$(find "$@" ! -size 0)
echo ""
echo "$(echo $failed | wc -w) failed tests"
echo ""
for a in $failed; do
    echo "**** $a"
    cat $a
    echo ""
done
exec test "$failed" == ""
