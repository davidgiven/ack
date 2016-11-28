#!/bin/sh
method=$1
img=$2
timeout=$3

pipe=/tmp/$$.testdriver.pipe
mknod $pipe p
trap "rm -f $pipe" EXIT

result=/tmp/$$.testdriver.result
trap "rm -f $result" EXIT

pidfile=/tmp/$$.testdriver.pid
trap "rm -f $pidfile" EXIT

case $method in
    qemu-system-*)
        if ! command -v $method >/dev/null 2>&1 ; then
            echo "Warning: $method not installed, skipping test"
            exit 0
        fi

        case $method in
            qemu-system-i386) img="-drive file=$img,if=floppy,format=raw" ;;
            qemu-system-ppc)  img="-kernel $img" ;;
        esac

        ( $method -nographic $img 2>&1 & echo $! > $pidfile ) \
            | tee $result \
            | ( timeout $timeout grep -l -q @@FINISHED ; echo ) \
            | ( read dummy && kill $(cat $pidfile) )
        
        ;;

    qemu-*)
        if ! command -v $method >/dev/null 2>&1 ; then
            echo "Warning: $method not installed, skipping test"
            exit 0
        fi

        $method $img > $result
        ;;

    *)
        echo "Error: $method not known by testdriver"
        exit 1
        ;;
esac

( grep -q @@FAIL $result || ! grep -q @@FINISHED $result ) && cat $result && exit 1
exit 0
