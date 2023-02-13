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
        qemu-*)
			errcho "qemu tests no longer supported (method $method)"
			exit 1
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
