#!/bin/sh

in=$1
header=$2
source=$3

echo "enum {" > $header
sed -n 's/^[A-Z].*$/IR_&,/p' < $in >> $header
echo "};" >> $header

echo "const char* ir_names[] = {" > $source
sed -n 's/^[A-Z].*$/"&",/p' < $in >> $source
echo "};" >> $source

