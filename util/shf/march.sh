: '$Id$'

case $# in
3) makecmd=$3 ;;
2) makecmd=compmodule ;;
*) echo "Usage: $0 srcdir archname [ makecmd ]"; exit 1 ;;
esac

errors=no
if test -r $1/LIST
then
  <$1/LIST (
    read archname
    if test -r $1/$archname
    then 
      arch x $1/$archname
      for file in `arch t $1/$archname`
      do
	echo $file: 1>&2
	suffix=`expr $file : '.*\(\..*\)'`
	ofile=`$makecmd $file $suffix`
	if test $? != 0
	then errors=yes
	fi
	rm $file
	OFILES="$OFILES $ofile"
      done
    else
      while read file
      do
	echo $file: 1>&2
	suffix=`expr $file : '.*\(\..*\)'`
	ofile=`$makecmd $1/$file $suffix`
	if test $? != 0
	then errors=yes
	fi
	OFILES="$OFILES $ofile"
      done
    fi
    if test $errors = no
    then
      if ${ASAR-arch} cr $2 $OFILES && ${RANLIB-:} $2
      then
        rm $OFILES
      fi
    else
      echo $2 not made, due to compilation errors
      exit 1
    fi
  )
else
  echo no LIST file in directory $1
  exit 1
fi
