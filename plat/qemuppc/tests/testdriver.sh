#!/bin/sh
qemu=$1
img=$2
timeout=$3

pipe=/tmp/$$.testdriver.pipe
mknod $pipe p
trap "rm -f $pipe" EXIT

result=/tmp/$$.testdriver.result
trap "rm -f $result" EXIT

pidfile=/tmp/$$.testdriver.pid
trap "rm -f $pidfile" EXIT

($qemu -nographic -kernel $img 2>&1 & echo $! > $pidfile ) | tee $result | \
    grep -l @@FINISHED | (read dummy && kill $(cat $pidfile))

grep @@FAIL $result && cat $result && exit 1
exit 0