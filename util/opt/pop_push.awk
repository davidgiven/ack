BEGIN	{ print "#define CONDBRA        '\001'" > "pop_push.h"
	  print "#define JUMP           '\002'" >> "pop_push.h"
	  print "#define HASLABEL       '\004'" >> "pop_push.h"
	  print "#include \"pop_push.h\""
	  print
	  print "char *pop_push[] = {"
	  print "\"\","
	  switch = 0
}
/aar/	{ switch = NR }
	{ if (switch) {
	      if ($1 == "cal" || $1 == "cai") $3 = "?"
	      printf("/* %s */ \"%s\",\n",$1,$3) 
	      col_2[NR-switch] = $2
	      comment[NR-switch] = $1
	  }
	}
END	{ print "};"
	  print
	  print "char flow_tab[]= {"
	  print "'\000',"
	  for(i=0; i < NR-switch; i++) {
		inf = col_2[i]
		f_out = "/* " comment[i] " */ "
		if (substr(inf,1,1)=="b") f_out = f_out "HASLABEL|"
		if (substr(inf,2,1)=="c") f_out = f_out "CONDBRA"
		else if (substr(inf,2,1)=="t") f_out = f_out "JUMP"
		else f_out = f_out "'\000'"
		print f_out","
	  }
	  print "};"
	}
