BEGIN		{
		FS = "|";
		seenproc = 0;
		CC="${CMD}"
		if (prototypes == "") prototypes = "prototypes"
		print "#include \"nopt.h\""
		}
/^%/		{}
/^$/		{}
/^[a-z]/ && $3 !~ /.*NOTIMPLEMENTED.*/ {
		if(seenproc) {
			print "}"
		}		
		seenproc = 1
		$1 = substr($1,1,index($1,"\t")-1);
		nam = $1
		print ""
		printf "void O_%s(",$1
		prototype = "_PROTOTYPE(void O_" $1 ", ("
		
		paramstr = $2
		gsub(/\t/,"",paramstr)
		gsub(/^[ \t]+/,"",paramstr)
		gsub(/[ \t]+$/,"",paramstr)	
		gsub(/:([A-Za-z0-9]+[ ])/,"&!",paramstr)
		
		
		count = split(paramstr,p2,"!");
		paramstr = ""
		if (count == 0)
		{
			printf "void"
			paramstr = paramstr "void"
		}
		else
		{	
			for (i=1;i<=count;i++)
			{
				if (length(p2[i]) > 0)
				{
					nparms = split(p2[i],params,":");
					if (nparms >0 )
					{
						gsub(/^[ \t]+/,"",params[1])
						gsub(/[ \t]+$/,"",params[1])	
						gsub(/^[ \t]+/,"",params[2])
						gsub(/[ \t]+$/,"",params[2])	
				
						paramstr = paramstr params[1] " " params[2]
						printf "%s %s",params[1],params[2]
						if(count!=i) 
						{
							paramstr = paramstr ", "
							printf ", "
						}
					} else
					{
						printf "void"
						paramstr = paramstr "void"
					}
				}
			}
		}
		prototype = prototype paramstr		
		print ")"
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
			print ""
		}
		}
