: '$Id$'

: Create a lint library file. The name of the library file is constructed
: from the first argument. The second argument indicates the directory where
: the result is to be placed. This version is for ACK lint.

n=$1
shift
d=$1
shift
lint -L$n $*
mv $n.llb $d
