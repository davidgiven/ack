BEGIN		{
		FS = "|";
		seenproc = 0;
		CC="${CMD}"
		}
/^%/		{}
/^$/		{}
/^[a-z]/ && $3 !~ /.*NOTIMPLEMENTED.*/ {
		if(seenproc) {
			print "}"
			print "--EOF--"
			printf "if %s C_%s.c\n",CC,nam
			printf "then :\nelse exit 1\nfi\n"
			printf "rm -f C_%s.c\n",nam
		}
		seenproc = 1
		$1 = substr($1,1,index($1,"\t")-1);
		nam = $1
		printf "cat > C_%s.c << '--EOF--'\n",$1
		print "#include \"nopt.h\""
		printf "C_%s(",$1
		nparms = split($2,parms,":");
		for(p=1;p<nparms;p++) {
			if(p!=1) {
				printf ","
			}
			split(parms[p+1],a," ")
			printf a[1]
		}
		printf ")\n"
		if(nparms) {
			printf "\t%s",parms[1]
		}
		for(p=1;p<nparms;p++) {
			split(parms[p+1],a," ")
			printf " %s;\n",a[1]
			if(a[2]) {
				printf "\t%s%s%s",a[2],a[3],a[4]
			}
		}
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
			printf "if %s C_%s.c\n",CC,nam
			printf "then :\nelse exit 1\nfi\n"
			printf "rm -f C_%s.c\n",nam
		}
		}
