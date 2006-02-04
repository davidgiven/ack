#!/bin/sh
# $Id$
#
# Script to rebuild LLgen's own parser with LLgen.

set -e

if ! (which LLgen > /dev/null); then
	echo "You can only bootstrap the LLgen parser if you've already got"
	echo "LLgen installed."
fi

(cd src && LLgen -vvv -x tokens.g LLgen.g)

echo ""
echo "You should now be able to rebuild LLgen with the new parser."
