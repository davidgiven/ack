BEGIN		{
		FS = "|";
		seenproc = 0;
		CC="${CMD}"
		if (prototypes == "") prototypes = "prototypes"
		}
/^%/		{}
/^$/		{}
/^[a-z]/ && $3 !~ /.*NOTIMPLEMENTED.*/ {
		if(seenproc) {
			print "}"
			print "--EOF--"
			printf "if %s O_%s.c\n",CC,nam
			printf "then :\nelse exit 1\nfi\n"
			printf "rm -f O_%s.c\n",nam
		}
		seenproc = 1
		$1 = substr($1,1,index($1,"\t")-1);
		nam = $1
		printf "cat > O_%s.c << '--EOF--'\n",$1
		print "#include \"nopt.h\""
		printf "void O_%s(",$1
		prototype = "_PROTOTYPE(void O_" $1 ", ("
		nparms = split($2,parms,":");
		for(p=1;p<nparms;p++) {
			if(p!=1) {
				printf ","
			}
			split(parms[p+1],a," ")
			printf a[1]
		}
		printf ")\n"
		if(nparms > 1) {
			prototype = prototype parms[1]
			printf "\t%s",parms[1]
		}
		else {
			prototype = prototype "void"
		}
		for(p=1;p<nparms;p++) {
			split(parms[p+1],a," ")
			prototype = prototype " " a[1]
			printf " %s;\n",a[1]
			if(a[2]) {
				prototype = prototype ", " a[2] a[3] a[4]
				printf "\t%s%s%s",a[2],a[3],a[4]
			}
		}
		print prototype "));" >> prototypes
		if($3) {
			printf "{\n\t%s\n",$3
		}
		else {
			printf "{\n"
		}
		}
/^	/	{
		print
		}
END		{
		if(seenproc) {
			print "}"
			print "--EOF--"
			printf "if %s O_%s.c\n",CC,nam
			printf "then :\nelse exit 1\nfi\n"
			printf "rm -f O_%s.c\n",nam
		}
		}
