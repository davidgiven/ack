BEGIN	{	printf(".TS\n");
		for (i = 0; i < 3; i++)
			printf("lw(4) 0 lw(6) 0 rw(2) 0 rw(5) 8 ");
		printf(".\n");
	}
NF == 4	{ printf "%s\t%s\t%d\t%d",$1,$2,$3,$4 }
NF == 3	{ printf "%s\t%s\t\t%d",$1,$2,$3 }
	{ if ( NR%3 == 0 ) printf("\n") ; else printf("\t"); }
END	{	if ( NR%3 != 0 ) printf("\n");
		printf(".TE\n");
	}
