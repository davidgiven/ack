#!/bin/sh
qemu=$1
img=$2
timeout=$3

pipe=/tmp/testdriver.$$.pipe
mknod $pipe p
trap "rm $pipe" EXIT

timeout $timeout $qemu -nographic -kernel $img >$pipe 2>&1 &
pid=$!

status=0
while read line < $pipe; do
    case "$line" in
        *@@FAIL*)
            echo $line
            status=1
            ;;

        *@@FINISHED*)
            kill $pid
            ;;
    esac
done

exit $status
