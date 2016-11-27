char(s, n)
	return((s[n/4]>>8*(n%4))&0377); /* s[n/4] */

lchar(s, n, char) {
	auto i;
	i = 8*(n%4);
	char = (char&0377)<<i;
	i = inv(0377<<i);
	s[(n/4)*4] = s[n/4]&i | char;
}

putchar(char) {
	auto c, i;

	c = char;
	i = 4;
	while ((c&0377) != '*e' & (c&0377) != '*0' & i != 0) {
		i--;
		c =>> 8;
	}
	write(1, &char, 4-i);
	return(char);
}

getchar() {
	auto char;

	char = 0;
	read(1, &char, 1);
	return(char);
}

printn(n,b) {
	extrn putchar;
	auto a;

	if (a = n/b)
		printn(a, b);
	putchar(char("0123456789ABCDEF", n%b));
}

putnumb(n) {
	printn(n,10);
	putchar('*n');
}

putstr(s) {
	auto c, i;

	i = 0;
	while ((c = char(s,i++)) != '*e')
		putchar(c);
}

getstr(s) {
	auto c, i;

	while ((c = getchar()) != '*n')
		lchar(s,i++,c);
	lchar(s,i,'*e');
	return(s);
}

printf(fmt, x1,x2,x3,x4,x5,x6,x7,x8,x9) {
	extrn printn, char, putchar;
	auto adx, x, c, i, j;

	i = 0;
	adx = &x1;
loop:
	while((c=char(fmt,i++)) != '%') {
		if(c == '*e')
			return;
		putchar(c);
	}
	x = *adx++;
	switch (c = char(fmt,i++)) {

	case 'd':
	case 'o':
		if(x < 0) {
			x = -x;
			putchar('-');
		}
		printn(x, c=='o'?8:10);
		goto loop;

	case 'x':
		if(x < 0) {
			x = -x;
			putchar('-');
		}
		printn(x, 16);
		goto loop;

	case 'c':
		putchar(x);
		goto loop;

	case 's':
		j = 0;
		while((c=char(x,j++)) != '*e')
			putchar(c);
		goto loop;
	}
	putchar('%');
	i--;
	adx--;
	goto loop;
}

