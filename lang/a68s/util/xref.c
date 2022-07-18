/*
 * xref makes cross references.
 *   November 1977		Johan Stevenson
 */

#include	<stdio.h>
#include	<signal.h>
#include	<setjmp.h>

/* type of flags() calls */
#define	HEAD	0
#define	TAIL	1

FILE	*input;
FILE	*output;
FILE	*hashin;
jmp_buf	env;		/* used by setjmp and longjmp */
int	scanout[2];	/* descriptor of output of scan */
int	postin[2];	/* descriptor of input of post */
int	ch;		/*last char*/
int	chsy;		/*type of last char*/
char	id[80];		/*last identifier*/
char	fl[80];		/*last filename (see post) */
char	buf[80];	/*work space*/
int	proc	= 0;	/*process id of sort*/
int	nflag;		/*line number flag*/
int	nfiles;
int	argc;
char	**argv;
char	*procname;
char	*file;		/*points to current file*/
int	pass1	= 1;
int	pass2	= 1;
int	only	= 0;	/* 1 if only selected words needed */
int	useroif	= 0;	/* 1 if user supplied ignore/only file*/
char	*oifile	= "/usr/lib/xrefign.\0";
int	oifsuf = 0;	/* index in oifile of last char */
int	linecount;
int	width	= 72;	/*line width*/
int	type;		/* which scanner must be used */
int	forced	= 0;	/* scanner type chosen by user */

stop()
{	
	if (proc!=0)
		kill(proc,9);
	exit(-1);
}

main(narg,args) char **args; 
int narg;
{
	argc=narg; 
	argv = args;
	argc--; 
	argv++;
	if (signal(SIGHUP,stop) != SIG_DFL)
		signal(SIGHUP,SIG_IGN);
	if (signal(SIGINT,stop) != SIG_DFL)
		signal(SIGINT,SIG_IGN);
	while (argc && argv[0][0]=='-' && argv[0][1]!='\0')
	{
		argc--; 
		flags(*argv++,HEAD);
	}
	if (argc==0) {
		argc++;
		*--argv = "-";
	}
	if (pass1 && pass2) {
		if (pipe(scanout)<0 || pipe(postin)<0)
			fatal("pipe failed");
		if ((proc=fork()) == 0) {
			close(0); 
			close(1);
			dup(scanout[0]); 
			dup(postin[1]);
			close(scanout[0]); 
			close(scanout[1]);
			close(postin[0]); 
			close(postin[1]);
			execl("/bin/sort","xref","+1","-3","+0n",0); 
			execl("/usr/bin/sort","xref","+1","-3","+0n",0);
			fatal("sort not found");
		}
		if (proc == -1) fatal("fork failed");
		close(scanout[0]); 
		close(postin[1]);
	}
	else if (pass1)
		scanout[1] = dup(1);
	else if (pass2)
		postin[0] = dup(0);
	if (pass1) {
		if (useroif) {
			if ((hashin = fopen(oifile, "rb")) == NULL)
				fatal("bad ignore/only file: %s",oifile);
			buildhash();
			fclose(hashin);
		}
		input = stdin;
		output = fdopen(scanout[1], "wb");
		nfiles = argc;
		setjmp(env);
		while (argc--)
			if (argv[0][0] == '-' && argv[0][1] != '\0')
				flags(*argv++,TAIL);
			else
				scan(*argv++);
		fclose(input);
		fclose(output);
	}
	if (pass2) {
		input = fdopen(postin[0], "r");
		output = stdout;
		post();
	}
	exit(0);
}

flags(s,ftype) register char *s;
{
	register c;

	s++;	/* skip - */
	switch (c = *s++) {
	case 'p':
	case '8':
	case 'c':
	case 's':
	case 'x':
		forced++; 
		type = c; 
		break;
	case '1':
		if (ftype == TAIL)
			fatal("-1 must precede file arguments");
		pass2=0; 
		pass1++; 
		break;
	case '2':
		if (ftype == TAIL)
			fatal("-2 must precede file arguments");
		pass1=0; 
		pass2++; 
		break;
	case 'i':
	case 'o':
		only = (c == 'o'); 
		useroif++;
		if (*s == '\0')
			fatal("more args expected");
		oifile = s;
		return;
	case 'w':
		if (*s == '\0')
			fatal("more args expected");
		width=atoi(s);
		return;
	default:
		fatal("possible flags: cpsxio12w");
	}
	if (*s != '\0')
		fatal("flags should be given as separate arguments");
}

char *tail(s)
register char *s;
{
	register char *t;

	t = s;
	while (*s)
		if (*s++ == '/')
			t = s;
	return(t);
}

scan(s) char *s;
{
	register lastc;

	linecount = 0; 
	nflag = 0;
	chsy = 0;
	if (nfiles==1)
		file = "";
	else
		file = tail(s);
	if (forced==0) {
		lastc = suffix(s);
		if (lastc=='h')
			lastc = 'c';
		if (lastc=='c' || lastc=='p' || lastc=='s' || lastc=='8')
			type=lastc;
		else
			type='x';
	} else
		lastc = type;
	if (useroif==0) {
		if (oifsuf == 0)
			while (oifile[oifsuf] != '\0')
				oifsuf++;
		if (lastc != oifile[oifsuf] ) {
			oifile[oifsuf] = lastc;
			if ((hashin = fopen(oifile, "rb")) == NULL) {
				oifile[oifsuf] = 'x';
				if ((hashin = fopen(oifile, "rb")) == NULL)
					fatal("cannot open %s",oifile);
			}
			buildhash();
			fclose(hashin);
		}
	}
	if (s[0]=='-' && s[1]=='\0')
		input = stdin;
	else
		if ((input = fopen(s, "rb")) == NULL)
			fatal("cannot open %s",s);
	switch (type) {
	case 'x': 
		x_scan(); 
		break;
	case 'p': 
		p_scan(); 
		break;
	case '8':
		a_scan();
		break;
	case 'c': 
		c_scan(); 
		break;
	case 's': 
		s_scan(); 
		break;
	}
	/*this place is never reached*/
}

suffix(s)
register char *s;
{
	while (*s) s++;
	if (*(s-2) == '.')
		return(*--s);
	return('x');
}

fatal(s) char *s;
{
	fprintf(stderr, "xref: %s",s);
	fprintf(stderr, "\n");
	stop();
}

/*============================================*/

#define HSIZE	79

struct { 
	int integ; 
};

struct link {
	struct link *next;
	char word[];
} 
*hashtab[HSIZE];

buildhash()
{
	register struct link *p,*q; 
	register char *s;
	int i;

	for (i=0; i<HSIZE; i++)
	{
		p = hashtab[i];
		hashtab[i] = 0;
		while (q = p)
		{
			p = q->next;
			free(q);
		}
	}
	ch = getc(hashin);
	while (ch != EOF) {
		s = id;
		do {
			*s++ = ch; 
			ch = getc(hashin);
		} while (ch>' ');
		*s++ = '\0';
		h_add(id,s-id);
		while (ch!='\n' && ch!=EOF)
			ch = getc(hashin);
		ch = getc(hashin);
	}
}


h_add(s,l) char *s; 
int l;
{
	register struct link *q,**p; 
	char temp[80];
	char *s2;

	if (h_in(s)) return;
	s2 = temp;
	strcpy(s2,s);
	if (strlen(s2)<=2)
		strcat(s2,"zz\0");
	p = &hashtab[ s2->integ % HSIZE ];
	l += 4+((4-(l & 3) & 3));
	if ((q = malloc(l)) == 0)
		fatal("out of space");
	q->next = *p;
	*p = q;
	strcpy(q->word, s);
}

h_in(s) char *s;
{
	register struct link *p;
	char temp[80];
	char *s2;

	s2 = temp;
	strcpy(s2,s);
	if (strlen(s)<= 2)
		strcat(s2,"zz\0");
	p = hashtab[ s2->integ % HSIZE ];
	while (p) {
		if (strcmp(s, p->word) == 0)
			return(1);
		p = p->next;
	}
	return(0);
}

/*=====================================*/

#define NL	-1
#define	ERROR	0
#define	LETTER	1
#define	DIGIT	2
#define	QUOTE	3
#define	LPAR	4
#define	LBRACE	5
#define DQUOTE	6
#define SLASH	7
#define POINT	9
#define LESS	10
#define USCORE	11
#define	OTHER	12
#define HASH 	13


char	cs[128] = {
	/*NUL*/	ERROR,	OTHER,	OTHER,	OTHER,	OTHER,	OTHER,	OTHER,	OTHER,
	/*010*/	OTHER,	OTHER,	NL,	OTHER,	OTHER,	OTHER,	OTHER,	OTHER,
	/*020*/	OTHER,	OTHER,	OTHER,	OTHER,	OTHER,	OTHER,	OTHER,	OTHER,
	/*030*/	OTHER,	OTHER,	OTHER,	OTHER,	OTHER,	OTHER,	OTHER,	OTHER,
	/*' '*/	OTHER,	OTHER,	DQUOTE,	HASH,	OTHER,	OTHER,	OTHER,	QUOTE,
	/*'('*/	LPAR,	OTHER,	OTHER,	OTHER,	OTHER,	OTHER,	POINT,	SLASH,
	/*'0'*/	DIGIT,	DIGIT,	DIGIT,	DIGIT,	DIGIT,	DIGIT,	DIGIT,	DIGIT,
	/*'8'*/	DIGIT,	DIGIT,	OTHER,	OTHER,	LESS,	OTHER,	OTHER,	OTHER,
	/*'@'*/	OTHER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,
	/*'H'*/	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,
	/*'P'*/	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,
	/*'X'*/	LETTER,	LETTER,	LETTER,	OTHER,	OTHER,	OTHER,	OTHER,	USCORE,
	/*'`'*/	OTHER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,
	/*'h'*/	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,
	/*'p'*/	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,
	/*'x'*/	LETTER,	LETTER,	LETTER,	LBRACE,	OTHER,	OTHER,	OTHER,	OTHER
};

nextch()
{	
	if (linecount == 0) {
		if ((ch=getc(input))==EOF) {
			fclose(input);
			longjmp(env,0);
		}
		else {
			chsy = cs[ch];
			if (chsy != DIGIT) 
				linecount++;
			else {
				nflag = 1;
				linecount = ch-'0';
				chsy = cs[(ch=getc(input))];
				while (chsy == DIGIT) {
					linecount = linecount*10+ch-'0';
					chsy = cs[(ch=getc(input))];
				}
			}
		}
	}
	else {
		if ((ch=getc(input))==EOF) {
			fclose(input);
			longjmp(env,0);
		}
		if (chsy < 0) {
			if (nflag == 0)
				linecount++;
			else {
				linecount = ch-'0';
				chsy = cs[(ch=getc(input))];
				while (chsy == DIGIT) {
					linecount = linecount*10+ch-'0';
					chsy = cs[(ch=getc(input))];
				}
			}
		}
		if (ch >= 128)
			fatal("bad chars on file %s",*--argv);
		chsy = cs[ch];
	}
}

out(p)
char *p;
{	
	fprintf(output, "%d	%s	%s\n",linecount,p,file);
}

scannumber()
{
	do nextch(); while (chsy == DIGIT);
	if (ch == '.') {
		nextch();
		if (chsy!=DIGIT) return;
		do nextch(); while (chsy == DIGIT);
	}
	if (ch == 'e') {
		nextch();
		if (ch == '+' || ch == '-')
			nextch();
		while (chsy == DIGIT)
			nextch();
	}
}

scansymbol(ok1,ok2) {
	register char *p;

	p = id;
	do {	
		*p++ = ch; 
		nextch();
	} while (chsy==LETTER || chsy==DIGIT || ch==ok1 || ch==ok2);
	*p = '\0';
	if (h_in(id) == only)
		out(id);
}

scanusymbol(ok1,ok2) {
	register char *p;

	p = id;
	do {	
		if (ch >= 'a' && ch <= 'z')
			ch += 'A'-'a';
		*p++ = ch; 
		nextch();
	} while (chsy==LETTER || chsy==DIGIT || ch==ok1 || ch==ok2);
	*p = '\0';
	if (h_in(id) == only)
		out(id);
}

escaped() {	
	if (ch=='\\') nextch(); 
	nextch();
}

comment(lastch) {	
	nextch();
	if (ch=='*') {
		nextch();
		do {
			while(ch!='*') nextch();
			nextch();
		} while (ch!=lastch);
		nextch();
	}
}

acmnt1() {

	/* handle a .COMMENT ..... .COMMENT */

	register char *p;
	register int cont;

	p = id;
	nextch();
	if (chsy==DIGIT) scannumber();
	else {
		do {
			*p++ = ch;
			nextch();
		} while (chsy==LETTER);
		/* see if the word is COMMENT */
		*p = '\0';
		p = id;
		if (strcmp("COMMENT",p)) { /* skip to next .COMMENT */
			cont = 1;
			while (cont) {
				while (chsy != POINT) nextch();
				nextch();
				p = id;
				do {
					*p++ = ch;
					nextch();
				} while (chsy==LETTER);
				*p = '\0';
				p = id;
				cont = strcmp("COMMENT",p);
			}
		}
		else { /* do hash lookup - could be pragmat (ignore) or record field */
			if (h_in(id)==only)
				out(id);
		}
	}			
}

acmnt2() {
	register char *p;
	int cont;

	/* handle a CO ..... CO comment */

	p = id;
	*p++ = 'C';
	nextch();
	if (ch!='O')  { /* do a scansymbol */
		do {
			*p++ =ch;
			nextch();
		} while (chsy==LETTER || chsy==DIGIT || chsy==USCORE);
		if (h_in(id)==only) 
			out(id);
	}
	else { /* found a CO .... CO */
		cont = 1;
		while (cont) {
			while (ch!='C') nextch();
			nextch();
			cont = (ch!='O');
		}
		nextch();
	}
}

p_scan() {	
	nextch();
	for(;;) switch (chsy) {
	case LETTER: 
	case USCORE:
		scanusymbol('_','\0'); 
		break;
	case DIGIT:
		scannumber(); 
		break;
	case QUOTE:
		do nextch(); while (ch!='\''); 
		nextch(); 
		break;
	case DQUOTE:
		do nextch(); while (ch!='"'); 
		nextch(); 
		break;
	case LPAR:
		comment(')'); 
		break;
	case LBRACE:
		do nextch(); while (ch!='}');
	default:
		nextch();
	}
}

a_scan() {	
	nextch();
	for(;;) switch (chsy) {
	case LETTER: 
		if (ch=='C') acmnt2();
		else
		scanusymbol('_','\0'); 
		break;
	case DIGIT:
		scannumber(); 
		break;
	case QUOTE:
		do nextch(); while (ch!='\''); 
		nextch(); 
		break;
	case DQUOTE:
		do nextch(); while (ch!='"'); 
		nextch(); 
		break;
	case HASH:
		nextch();
		while (ch!='#') nextch();
		nextch();
		break;
	case POINT:
		acmnt1();
		break;
	default:
		nextch();
	}
}

c_scan()
{	
	nextch();
	for (;;) switch (chsy) {
	case LETTER: 
	case USCORE:
		scansymbol('_','\0'); 
		break;
	case DIGIT:
		scannumber(); 
		break;
	case SLASH:
		comment('/'); 
		break;
	case QUOTE:
		do escaped(); while (ch!='\''); 
		nextch(); 
		break;
	case DQUOTE:
		do escaped(); while (ch!='"');
	default:
		nextch();
	}
}

s_scan()
{	
	nextch();
	for(;;) switch(chsy) {
	case LETTER: 
	case POINT:
		scansymbol('_','.'); 
		break;
	case DIGIT:
		do nextch(); while (chsy==DIGIT);
		if (ch=='.' || ch=='f' || ch=='b') nextch();
		break;
	case DQUOTE:
		nextch();
	case QUOTE:
		escaped(); 
		escaped(); 
		break;
	case SLASH:
		do nextch(); while (ch!='\n'); 
		break;
	case LESS:
		nextch();
		do escaped(); while (ch!='>');
		break;
	default:
		nextch();
	}
}

x_scan()
{
	register char *p;
	nextch();
	for (;;) switch (chsy) {
	case LETTER:
		p=id;
		do {	
			if (ch<'A' || ch>'Z') *p++ = ch;
			else *p++ = ch - 'A' + 'a';
			nextch();
			if (ch=='-') {
				nextch();
				if (ch=='\n')
					do nextch(); while (chsy!=LETTER);
				else *p++ = '-';
			}
		} while (chsy==LETTER || chsy==DIGIT);
		*p = '\0';
		if (h_in(id) == only) out(id);
		break;
	default:
		nextch();
	}
}

/*=========================================*/

int N;

post()
{
	register n,l,i; 
	int first,newid,newfl,withfile;

	first = 1; 
	id[0] = '\0';
	ch = getc(input);
	while (ch != EOF) {
		l = getfld('\t');
		if ((i=atoi(buf)) == 0)
			fatal("line number expected");
		l = getfld('\t');
		newid = strcmp(id,buf);
		if (newid) {
			strcpy(id,buf);
			if (first == 0)
				putc('\n',output);
			fprintf(output,"%s",id);
			if (l > 7)
				putc('\n',output);
			putc('\t',output);
			fl[0] = '\0';
		}
		l = getfld('\n');
		newfl = strcmp(fl,buf);
		if (newfl) {
			strcpy(fl,buf);
			if (newid == 0)
				fprintf(output,"\n\t");
			fprintf(output,"%s",fl);
			if (l > 7)
				fprintf(output,"\n\t");
			putc('\t',output);
		}
		if (first) {
			first = 0;
			withfile = newfl;
			N = width - 12;
			if (withfile) N -= 8;
			if (N<0) fatal("line width too small");
			N = (N/5) + 1;
		}
		if (newid || newfl)
			n = N;
		else if (n==0) {
			fprintf(output,"\n\t");
			if (withfile)
				putc('\t',output);
			n = N;
		}
		else
			putc(' ',output);
		n--;
		fprintf(output,"%4d",i);
	}
	putc('\n',output);
}

getfld(stopch) {
	register char *p;

	p = buf;
	while (ch!=EOF && ch!=stopch) {
		*p++ = ch;
		ch = getc(input);
	}
	*p = '\0';
	ch = getc(input);
	return(p-buf);
}
