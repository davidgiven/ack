BEGIN { printf ".TS\nlw(6) lw(8) rw(3) rw(6) 14 lw(6) lw(8) rw(3) rw(6) 14 lw(6) lw(8) rw(3) rw(6).\n" }
NF == 4 { printf "%s\t%s\t%d\t%d",$1,$2,$3,$4 }
NF == 3 { printf "%s\t%s\t\t%d",$1,$2,$3 }
 { if ( NR%3 == 0 ) printf("\n") ; else printf("\t"); }
END { if ( NR%3 != 0 ) printf("\n")
      printf ".TE\n" }
