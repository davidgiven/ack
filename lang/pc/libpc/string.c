/* $Header$ */
/* function strbuf(var b:charbuf):string; */

char *strbuf(s) char *s; {
	return(s);
}

/* function strtobuf(s:string; var b:charbuf; blen:integer):integer; */

int strtobuf(s,b,l) char *s,*b; {
	int i;

	i = 0;
	while (--l>=0) {
		if ((*b++ = *s++) == 0)
			break;
		i++;
	}
	return(i);
}

/* function strlen(s:string):integer; */

int strlen(s) char *s; {
	int i;

	i = 0;
	while (*s++)
		i++;
	return(i);
}

/* function strfetch(s:string; i:integer):char; */

int strfetch(s,i) char *s; {
	return(s[i-1]);
}

/* procedure strstore(s:string; i:integer; c:char); */

strstore(s,i,c) char *s; {
	s[i-1] = c;
}
