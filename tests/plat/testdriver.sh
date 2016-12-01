#!/bin/sh
method=$1
img=$2
timeout=$3
timeoutprog=$4

set -e

result=/tmp/$$.testdriver.result
trap "rm -f $result" EXIT

errcho() {
    >&2 echo "$*"
}

get_test_output() {
    case $method in
        qemu-system-*)
            if ! command -v $method >/dev/null 2>&1 ; then
                errcho "Warning: $method not installed, skipping test"
                echo "@@SKIPPED"
                exit 0
            fi

            case $method in
                qemu-system-i386) img="-drive file=$img,if=floppy,format=raw" ;;
                qemu-system-ppc)  img="-kernel $img" ;;
            esac

            $timeoutprog -t $timeout -- $method -nographic $img 2>&1 > $result
            ;;

        qemu-*)
            if ! command -v $method >/dev/null 2>&1 ; then
                errcho "Warning: $method not installed, skipping test"
                echo "@@SKIPPED"
                exit 0
            fi

            $method $img 2>&1 > $result
            ;;

        *)
            errcho "Error: $method not known by testdriver"
            exit 1
            ;;
    esac
}

get_test_output
( grep -q '@@FAIL\|@@SKIPPED' $result || ! grep -q @@FINISHED $result ) && cat $result && exit 1
exit 0
