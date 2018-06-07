#!/bin/sh
method=$1
img=$2
timeout=$3
timeoutprog=$4

set -e

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

            $timeoutprog -t $timeout -- $method -nographic $img 2>&1
            ;;

        qemu-*)
            if ! command -v $method >/dev/null 2>&1 ; then
                errcho "Warning: $method not installed, skipping test"
                echo "@@SKIPPED"
                exit 0
            fi

            $method $img 2>&1
            ;;

        *)
			$timeoutprog -t $timeout -- $method $img 2>&1
            ;;
    esac
}

# Hide output if the test passed.
# Show output if it failed, skipped, or timed out.
get_test_output | awk '
    { lines[count++] = $0 }
    /@@FAIL|@@SKIPPED|@@TIMEDOUT/ { bad = 1 }
    /@@FINISHED/ { finished = 1 }
    END {
        if (finished && !bad) exit 0
        for (i = 0; i < count; i++) print lines[i]
        if (!bad) print "@@FAIL by testdriver.sh"
        exit 1
    }
'
