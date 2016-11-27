#include "b.h"

void extdef(void);
struct hshtab * lookup(void);
void blkhed(void);
void blkend(void);
void retseq(void);
void statement(int d);
struct tnode * tree(void);
void errflush(int o);

int	line = 1;
int	peeksym = -1, peeksym2 = -1;;
int	contlab = -1;
int	brklab = -1;

void
init(char *s, int val)
{
	char *sp;
	struct hshtab *np;

	sp = symbuf;
	while (sp < symbuf+NCPS+1)
		if ((*sp++ = *s++) == '\0')
			s--;
	np = lookup();
	np->class = KEYWF;
	np->offset = val;
}

int
main(int argc, char *argv[])
{
	if (argc < 3) {
		error("Arg count");
		exit(1);
	}
	if (freopen(argv[1], "r", stdin) == NULL) {
		error("Can't find %s", argv[1]);
		exit(1);
	}
	if ((sbufp=fopen(argv[2], "w")) == NULL) {
		error("Can't create %s", argv[2]);
		exit(1);
	}
	if (argc > 3) {
		if (freopen(argv[3], "w", stdout) == NULL) {
			error("Can't create %s", argv[2]);
			exit(1);
		}
	}
	init("auto", AUTO);
	init("extrn", EXTERN);
	init("case", CASE);
	init("if", IF);
	init("else", ELSE);
	init("while", WHILE);
	init("switch", SWITCH);
	init("goto", GOTO);
	init("return", RETURN);
	init("default", DEFAULT);
	init("break", BREAK);
	fprintf(sbufp, "\t.data\n");
	while (!eof) {
		extdef();
		blkend();
	}
	return nerror != 0;
}

/*
 * Lexer
 */

int
spnextchar(void)
{
	int c;

	if ((c = peekc) == 0)
		c = getchar();
	if (c == '\t')
		c = ' ';
	else if (c == '\n') {
		c = ' ';
		line++;
	}
	peekc = c;
	return c;
}

int
nextchar(void)
{
	while (spnextchar() == ' ')
		peekc = 0;
	return peekc;
}

int
subseq(int c, int a, int b)
{
	if (spnextchar() != c)
		return a;
	peekc = 0;
	return b;
}

/* Only decimal and octal bases, could extend */
int
getnum(void)
{
	int base;
	int c;

	base = 10;
	cval = 0;
	if ((c=spnextchar()) == '0')
		base = 8;
	for (; ctab[c] == DIGIT; c = getchar())
		cval = cval*base + c-'0';
	peekc = c;
	return CON;
}

int
mapch(char c)
{
	int a;

	if ((a=getchar()) == c)
		return -1;
	switch (a) {

	case '\n':
	case '\0':
		error("Nonterminated string");
		peekc = a;
		return -1;

	case '*':
		switch (a=getchar()) {

		case 't':
			return('\t');

		case 'n':
			return('\n');

		case '0':
			return('\0');

		case '(':
			return('{');

		case ')':
			return('}');

		case 'e':
			return(EOS);

		case '\n':
			line++;
			return('\n');
		}
	}
	return a;
}

int
getcc(void)
{
	char *cp;
	int c, cc;

	cval = 0;
	cc = 0;
	cp = (char*) &cval;
	while ((c = mapch('\'')) >= 0)
		if (cc++ < NCPW)
			*cp++ = c;
	if (cc > NCPW)
		error("Long character constant");
	return CON;
}

int
getstr(void)
{
	int c;
	int i;

	fprintf(sbufp, "\t.align 4\n");
	fprintf(sbufp, "L%d:", cval = isn++);
	if ((c = mapch('"')) >= 0)
		fprintf(sbufp, "\t.byte %04o", c);
	for (i = 2; (c = mapch('"')) >= 0; i++)
		fprintf(sbufp, ",%04o", c);
	fprintf(sbufp, ",04");
	while ((i++%4) != 0)
		fprintf(sbufp, ",00");
	fprintf(sbufp, "\n");
	return STRING;
}

struct hshtab *
lookup(void)
{
	int i;
	char *sp, *np;
	struct hshtab *rp;

	i = 0;
	sp = symbuf;
	while (sp < symbuf+NCPS)
		i += *sp++&0177;
	rp = &hshtab[i%HSHSIZ];
	while (*(np = rp->name)) {
		for (sp=symbuf; sp < symbuf+NCPS;)
			if (*np++ != *sp++)
				goto no;
		return rp;
	no:
		if (++rp >= &hshtab[HSHSIZ])
			rp = hshtab;
	}
	if (++hshused > HSHSIZ) {
		error("Symbol table overflow");
		exit(1);
	}
	rp->class = 0;
	rp->offset = 0;
	rp->dim = 0;
	sp = symbuf;
	for (np = rp->name; sp < symbuf+NCPS+1;)
		*np++ = *sp++;
	return rp;
}

/*
 * Symbol peeking with one peeksym doesn't work if an ASSIGN is only peeked,
 * since it itself peeks a symbol, which is then overwritten.
 */

/* Note: does not push bsyms !! */
int
pushsym(int sym)
{
	if (peeksym < 0)
		peeksym = sym;
	else if (peeksym2 < 0) {
		peeksym2 = peeksym;
		peeksym = sym;
	} else
		error("Cannot push more than two symbols\n");
	return sym;
}

int
symbol(void)
{
	int c;
	char *sp;

	if (peeksym >= 0) {
		c = peeksym;
		peeksym = peeksym2;
		peeksym2 = -1;
		return c;
	}
	if (peekc) {
		c = peekc;
		peekc = 0;
	} else
		if (eof)
			return EOFC;
		else
			c = getchar();
	if (c==EOF) {
		eof++;
		return(EOFC);
	}

loop:
	switch (ctab[c]) {

	case NEWLN:
		line++;

	case SPACE:
		c = getchar();
		goto loop;

	case PLUS:
		return subseq(c,PLUS,INCBEF);

	case MINUS:
		return subseq(c,MINUS,DECBEF);

	case LESS:
		if (subseq(c,0,1))
			return LSHIFT;
		return subseq('=', LESS, LESSEQ);

	case GREAT:
		if (subseq(c,0,1))
			return RSHIFT;
		return subseq('=', GREAT, GREATEQ);

	case ASSIGN:
		if (subseq(' ',0,1))
			return ASSIGN;
		/* avoid peeking a name, which could overwrite
		 * an already set bsym. */
		if (ctab[peekc = spnextchar()] == LETTER)
			return ASSIGN;
		c = symbol();
		if (PLUS <= c && c <= GREAT)
			return c + ASPLUS-PLUS;
		if (c == ASSIGN)
			return EQUAL;
		pushsym(c);
		return ASSIGN;

	case EXCLA:
		return subseq('=',EXCLA,NEQUAL);

	case DIVIDE:
		if (subseq('*',1,0))
			return DIVIDE;
		while ((c = spnextchar()) != EOFC) {
			peekc = 0;
			if (c == '*') {
				if (spnextchar() == '/') {
					peekc = 0;
					c = getchar();
					goto loop;
				}
			}
		}
		eof++;
		error("Nonterminated comment");
		return EOFC;

	case DIGIT:
		peekc = c;
		return getnum();

	case SQUOTE:
		return(getcc());

	case DQUOTE:
		return(getstr());

	case LETTER:
		sp = symbuf;
		while (ctab[c] == LETTER || ctab[c] == DIGIT) {
			if (sp < symbuf+NCPS)
				*sp++ = c;
			c = getchar();
		}
		while (sp < symbuf+NCPS+1)
			*sp++ = '\0';
		peekc = c;
		bsym = lookup();
		if (bsym->class == KEYWF) {
			cval = bsym->offset;
			return KEYW;
		}
		return NAME;

	case UNKN:
		error("Unknown character");
		c = getchar();
		goto loop;
	}
	return (ctab[c]);
}

/*
 * Declarations and Definitions
 */

/* Declares a list of names to be of storage class "kw". */
void
declare(int kw)
{
	int o;

	while ((o = symbol()) == NAME) {
		if (bsym->class)
			error("%s redeclared", bsym->name);
		bsym->class = kw;
		while ((o = symbol()) == LBRACK) {
			if ((o = symbol()) == CON) {
				if (bsym->dim)
					error("Bad vector");
				bsym->dim = cval + 1;
				o = symbol();
			}
			if (o != RBRACK)
				goto syntax;
		}
		if (kw == ARG) {
			bsym->next = NULL;
			if (!paraml)
				paraml = bsym;
			else
				parame->next = bsym;
			parame = bsym;
		}
		if (o != COMMA)
			break;
	}
	if ((o == SEMI && kw != ARG) || (o == RPARN && kw == ARG))
		return;
syntax:
	error("Declaration syntax");
	errflush(o);
}

void
declist(void)
{
	int o;

	while ((o = symbol()) == KEYW && (cval == AUTO || cval == EXTERN))
		declare(cval);
	pushsym(o);
}

void
function(void)
{
	printf("\tpush\t%%ebp\n");
	printf("\tmov\t%%esp,%%ebp\n");

	declare(ARG);
	statement(1);
	retseq();
}

void
global(char *s)
{
	printf("\t.globl\t_%s\n", s);
	printf("\t.data\n");
	printf("\t.align %d\n", ALIGN);
}

void
bsymb(char *s, int und)
{
	printf("\t.section .bsymb; .long %s%s; .data\n", und?"_":"", s);
}

void
extdef(void)
{
	int o, dim, i;
	char *bs;

	if ((o = symbol()) == EOFC || o == SEMI)
		return;
	if (o != NAME)
		goto syntax;
	bs = bsym->name;
	i = dim = 0;
	switch(o = symbol()) {

	case SEMI:
		printf("\t.comm\t_%s,%d,%d\n", bs, NCPW, ALIGN);
		goto done;

	/* init */
	case CON:
	case STRING:
		global(bs);
		if (o == STRING)
			bsymb(bs,1);
		printf("_%s:", bs);
		pushsym(o);
		goto init;

	/* vector */
	case LBRACK:
		if ((o=symbol()) == CON) {
			dim = cval + 1;
			o=symbol();
		}
		if (o != RBRACK)
			goto syntax;
		global(bs);
		if ((o=symbol()) == SEMI) {
			printf("\t.comm\tL%d,%d,%d\n", isn, dim*NCPW, ALIGN);
			bsymb(bs,1);
			printf("_%s:\t.long L%d\n", bs, isn++);
			goto done;
		}
		bsymb(bs,1);
		printf("_%s:\t.long 1f\n1:", bs);
		pushsym(o);

	init:
		do {
			if ((o=symbol()) != CON && o != STRING && o != NAME)
				goto syntax;
			if (o == NAME) {
				bsymb("1f",0);
				printf("1:\t.long _%s\n", bsym->name);
			} else
				printf("\t.long %s%d\n", o==STRING?"L":"",cval);
			i++;
		} while ((o=symbol()) == COMMA);
		dim = (i > dim) ? i : dim;
		if (dim - i)
			printf("\t.zero %d\n", (dim-i)*NCPW);
		if (o == SEMI)
			goto done;
		goto syntax;

	/* function */
	case LPARN:
		global(bs);
		bsymb(bs,1);
		printf("_%s:\t.long 1f\n", bs);
		printf("\t.text\n\t.align 4\n1:");
		function();
	done:
		printf("\n");
		return;

	case EOFC:
		return;
	}
syntax:
	error("External definition syntax");
	printtoken(o, stderr);
	errflush(o);
	statement(0);
}

void
setstk(int a)
{
	int dif;

	dif = stack-a;
	stack = a;
	if (dif)
		printf("\tsub\t$%d, %%esp\n", dif);
}

void
defvec(void)
{
	stack -= NCPW;
	printf("\tmov\t%%esp,%%eax\n");
	printf("\tshr\t$2,%%eax\n");
	printf("\tpush\t%%eax\n");
}

void
blkhed(void)
{
	int al, pl;
	struct hshtab *bs;

	declist();
	stack = al = -4;
	pl = 8;
	while (paraml) {
		paraml = (bs = paraml)->next;
		bs->offset = pl;
		pl += NCPW;
	}
	for (bs = hshtab; bs < &hshtab[HSHSIZ]; bs++)
		if (bs->name[0]) {
			if (bs->class == AUTO) {
				bs->offset = al;
				if (bs->dim) {
					al -= bs->dim*NCPW;
					setstk(al);
					defvec();
					bs->offset = al;
				}
				al -= NCPW;
			} else if (bs->class == ARG)
				bs->class = AUTO;
		}
	setstk(al);
}

void
blkend(void)
{
	struct hshtab *np;

	for (np = hshtab; np < &hshtab[HSHSIZ]; np++)
		if (np->class != KEYWF) {
			np->name[0] = '\0';
			hshused--;
		}
}

/*
 * Statements and Expressions
 */

struct tnode *
pexpr(void)
{
	struct tnode *t;
	int o;

	if ((o = symbol()) != LPARN)
		goto syntax;
	t = tree();
	if ((o = symbol()) != RPARN)
		goto syntax;
	return t;
syntax:
	error("Statement syntax");
	errflush(o);
	return NULL;
}

void
label(int l)
{
	printf("L%d:\n", l);
}

void
retseq(void)
{
	printf("\tjmp\tretrn\n");
}

/* Jump to "lab", if the expression "t" evaluated to "val". */
void
cbranch(struct tnode *t, int lab, int val)
{
	rcexpr(t);
	if (val == 0)
		printf("\ttest\t%%eax,%%eax\n");
	else
		printf("\tcmp\t%%eax,$%d\n", val);
	printf("\tje\tL%d\n", lab);
}

void
jump(int lab)
{
	printf("\tjmp\tL%d\n", lab);
}

void
pswitch(void)
{
	struct swtab *sswp;
	int dl, swlab;

	sswp = swp;
	if (swp == NULL)
		swp = swtab;
	swlab = isn++;
	printf("\tmov\t$L%d,%%ebx\n", swlab);
	printf("\tjmp\tbswitch\n");
	dl = deflab;
	deflab = 0;
	statement(0);
	if (!deflab) {
		deflab = isn++;
		label(deflab);
	}
	printf("L%d:\n\t.data\nL%d:", brklab, swlab);
	while (swp > sswp && swp > swtab) {
		--swp;
		printf("\t.long %d,L%d\n", swp->swval, swp->swlab);
	}
	printf("\t.long L%d,0\n", deflab);
	printf("\t.text\n");
	deflab = dl;
	swp = sswp;
}

void
statement(int d)
{
	int o, o1, o2;

stmt:
	if ((o = symbol()) == LBRACE) {
		if (d)
			blkhed();
		while (!eof) {
			if ((o = symbol()) == RBRACE)
				goto bend;
			pushsym(o);
			statement(0);
		}
		error("Missing '}'");
	bend:
		return;
	} else {
		pushsym(o);
		if (d)
			blkhed();
	}

	switch (o = symbol()) {

	case EOFC:
		error("Unexpected EOF");

	case SEMI:
	case RBRACE:
		return;

	case KEYW:
		switch (cval) {
		case GOTO:
			if ((o = symbol()) != NAME)
				goto syntax;
			jump(bsym->offset);
			goto semi;

		case RETURN:
			if (pushsym(symbol()) == LPARN)
				rcexpr(pexpr());
			retseq();
			goto semi;

		case IF:
			cbranch(pexpr(), o1=isn++, 0);
			statement(0);
			if ((o = symbol()) == KEYW && cval == ELSE) {
				jump(o2 = isn++);
				label(o1);
				statement(0);
				label(o2);
				return;
			}
			pushsym(o);
			label(o1);
			return;

		case WHILE:
			o1 = contlab;
			o2 = brklab;
			label(contlab = isn++);
			cbranch(pexpr(), brklab=isn++, 0);
			statement(0);
			jump(contlab);
			label(brklab);
			contlab = o1;
			brklab = o2;
			return;

		case BREAK:
			if (brklab < 0)
				error("Nothing to break from");
			jump(brklab);
			goto semi;

		/* Not part of B, but very easy to implement */
/*
		case CONTINUE:
			if (contlab < 0)
				error("Nothing to continue");
			jump(contlab);
			goto semi;
*/

		case SWITCH:
			o1 = brklab;
			brklab = isn++;
			rcexpr(pexpr());
/*			rcexpr(tree()); */
			pswitch();
			brklab = o1;
			return;

		case CASE:
			if ((o = symbol()) != CON)
				goto syntax;
			if ((o = symbol()) != COLON)
				goto syntax;
			if (swp == NULL) {
				error("Case not in switch");
				goto stmt;
			}
			if (swp >= swtab+SWSIZ)
				error("Switch table overflow");
			else {
				swp->swlab = isn;
				(swp++)->swval = cval;
				label(isn++);
			}
			goto stmt;

		case DEFAULT:
			if (swp == NULL)
				error("Default not in switch");
			if ((o = symbol()) != COLON)
				goto syntax;
			deflab = isn++;
			label(deflab);
			goto stmt;
		}

		error("Unknown keyword");
		goto syntax;

	case NAME:
		if (peekc == ':') {
			peekc = 0;
			if (bsym->class) {
				error("Redefinition");
				goto stmt;
			}
			bsym->class = INTERN;
			bsym->offset = isn++;
			label(bsym->offset);
			goto stmt;
		}
	}
	pushsym(o);
	rcexpr(tree());
	goto semi;

semi:
	if ((o = symbol()) != SEMI)
		goto syntax;
	return;

syntax:
	error("Statement syntax");
	errflush(o);
	goto stmt;
}

struct tnode *
block(int op, int value, struct tnode *tr1, struct tnode *tr2)
{
	struct tnode t;
	int n;
	int *p, *ap;

	p = space;
	t.op = op;
	t.value = value;
	t.tr1 = tr1;
	t.tr2 = tr2;
	ap = (int*) &t;
	n = (sizeof(struct tnode)+sizeof(int)-1) & ~sizeof(int);
	if (space+n >= &ospace[OSSIZ]) {
		error("Expression overflow 1");
		exit(1);
	}
	while (n--)
		*space++ = *ap++;
	return (struct tnode *) p;
}

void
chklval(struct tnode *p)
{
	if (p->op != NAME && p->op != STAR)
		error("Lvalue required");
}

void
build(int op)
{
	struct tnode *p1, *p2;
	int dope;

	/* a[i] -> *(a+i) */
	if (op == LBRACK) {
		build(PLUS);
		op = STAR;
	}
	dope = opdope[op];
	if (dope&01)
		p2 = *--cp;
	p1 = *--cp;
	switch (op) {
	case QUEST:
		if (p2->op != COLON)
			error("Illegal conditional");
		break;

	case AMPER:
		if (p1->op == STAR) {
			*cp++ = p1->tr1;
			return;
		}
		if (p1->op == NAME) {
			*cp++ = block(op,0,p1,NULL);
			return;
		}
		error("Illegal lvalue");
	}
	if (dope&02)
		chklval(p1);
	if (dope&01)
		*cp++ = block(op,0,p1,p2);
	else
		*cp++ = block(op,0,p1,NULL);
}

struct tnode *
tree(void)
{
	struct tnode *cmst[CMSIZ];
	int opst[SSIZE], prst[SSIZE];
	int *op, *pp;
	int andflg;
	int o, os;
	int p, ps;

	space = ospace;
	op = opst;
	pp = prst;
	cp = cmst;
	*op = SEOF;
	*pp = 06;
	andflg = 0;

advanc:
	switch (o=symbol()) {
	case NAME:
		if (pushsym(symbol()) == LPARN) {	/* function */
			bsym->class = EXTERN;
		} else if (bsym->class == 0) {
			error("%s undefined", bsym->name);
			bsym->class = EXTERN;
		}
		*cp++ = block(NAME,0,(struct tnode *)bsym,NULL);
		goto tand;

	case STRING:
		*cp++ = block(STRING,cval,NULL,NULL);
		goto tand;

	case CON:
	caseCON:
		*cp++ = block(CON,cval,NULL,NULL);
		goto tand;

tand:
		if (cp >= &cmst[CMSIZ]) {
			error("Expression overflow 2");
			exit(1);
		}
		if (andflg)
			goto syntax;
		andflg = 1;
		goto advanc;

	case DECBEF:
	case INCBEF:
		if (andflg)
			o += 2;
		goto oponst;

	case EXCLA:
		if (andflg)
			goto syntax;
		goto oponst;

	case MINUS:
		if (!andflg) {
			if (pushsym(symbol()) == CON) {
				symbol();
				cval = -cval;
				goto caseCON;
			}
			o = NEG;
		}
		andflg = 0;
		goto oponst;

	case AND:
	case TIMES:
		if (andflg)
			andflg = 0;
		else
			if (o == AND)
				o = AMPER;
			else
				o = STAR;
		goto oponst;

	case LPARN:
		if (andflg) {
			o = symbol();
			if (o == RPARN)
				o = MCALL;
			else {
				pushsym(o);
				o = CALL;
				andflg = 0;
			}
		}
		goto oponst;

	case RPARN:
	case RBRACK:
		if (!andflg)
			goto syntax;
		goto oponst;
	}

	if (!andflg)
		goto syntax;
	andflg = 0;

oponst:
	p = (opdope[o]>>9) & 077;
opon1:
	ps = *pp;
	if (p > ps || (p == ps && (opdope[o]&0200))) {	/* right-assoc */
		switch (o) {
		case LPARN:
		case LBRACK:
		case CALL:
			p = 04;
		}
		if (op >= &opst[SSIZE]) {
			error("Expression overflow 3");
			exit(1);
		}
		*++op = o;
		*++pp = p;
		goto advanc;
	}
	--pp;
	switch (os = *op--) {
	case SEOF:
		pushsym(o);
		return(*--cp);

	case CALL:
		if (o != RPARN)
			goto syntax;
		build(os);
		goto advanc;

	case MCALL:
		*cp++ = NULL;
		os = CALL;
		goto fbuild;

	case LPARN:
		if (o != RPARN)
			goto syntax;
		goto advanc;

	case LBRACK:
		if (o != RBRACK)
			goto syntax;
		build(LBRACK);
		goto advanc;

	}
fbuild:
	build(os);
	goto opon1;
syntax:
	error("Expression syntax");
	errflush(o);
	return NULL;
}

void
error(char *s, ...)
{
	va_list args;

	va_start(args, s);
	nerror++;
	fprintf(stderr, "%d: ", line);
	vfprintf(stderr, s, args);
	putc('\n', stderr);
	va_end(args);
}

void
errflush(int o)
{
	while (o > RBRACE)	/* ; { } */
		o = symbol();
	pushsym(o);
}

/*
 *	000001	binary
 *	000002	need lvalue
 *	000004	relational
 *	000010	assignment
 *	000100	commutative
 *	000200	right-assoc
 *	0XX000	precedence
 */
int opdope[] = {
	000000,	/* EOFC */
	000000,	/* ; */
	000000,	/* { */
	000000,	/* } */
	036000,	/* [ */
	002000,	/* ] */
	036000,	/* ( */
	002000,	/* ) */
	014201,	/* : */
	007001,	/* , */
	000000,	/* 10 */
	000000,	/* 11 */
	000000,	/* 12 */
	000000,	/* 13 */
	000000,	/* 14 */
	036001,	/* mcall */
	036001,	/* call */
	034202,	/* ++a */
	034202,	/* a++ */
	034202,	/* --a */
	034202,	/* a-- */
	034200,	/* !un */
	034200,	/* -un */
	034200,	/* &un */
	034200,	/* *un */
	014201,	/* ? */
	000000,	/* 26 */
	000000,	/* 27 */
	000000,	/* 28 */
	000000,	/* 29 */
	030101,	/* + */
	030001,	/* - */
	032001,	/* % */
	032101,	/* * */
	032001,	/* / */
	016101,	/* | */
	020101,	/* & */
	026001,	/* << */
	026001,	/* >> */
	022105,	/* == */
	022105,	/* != */
	024005,	/* <= */
	024005,	/* < */
	024005,	/* >= */
	024005,	/* > */
	000000,	/* 45 */
	000000,	/* 46 */
	000000,	/* 47 */
	000000,	/* 48 */
	012013,	/* = */
	012213,	/* =+ */
	012213,	/* =- */
	012213,	/* =% */
	012213,	/* =* */
	012213,	/* =/ */
	012213,	/* =| */
	012213,	/* =& */
	012213,	/* =<< */
	012213,	/* =>> */
	012213,	/* === */
	012213,	/* =!= */
	012213,	/* =<= */
	012213,	/* =< */
	012213,	/* =>= */
	012213,	/* => */
	000000,	/* CON */
	000000,	/* STRING */
	000000	/* NAME */
};

char ctab[128] = {
	EOFC,	UNKN,	UNKN,	UNKN,	UNKN,	UNKN,	UNKN,	UNKN,
	LETTER,	SPACE,	NEWLN,	SPACE,	SPACE,	UNKN,	UNKN,	UNKN,
	UNKN,	UNKN,	UNKN,	UNKN,	UNKN,	UNKN,	UNKN,	UNKN,
	UNKN,	UNKN,	UNKN,	UNKN,	UNKN,	UNKN,	UNKN,	UNKN,
	SPACE,	EXCLA,	DQUOTE,	UNKN,	UNKN,	MOD,	AND,	SQUOTE,
	LPARN,	RPARN,	TIMES,	PLUS,	COMMA,	MINUS,	UNKN,	DIVIDE,
	DIGIT,	DIGIT,	DIGIT,	DIGIT,	DIGIT,	DIGIT,	DIGIT,	DIGIT,
	DIGIT,	DIGIT,	COLON,	SEMI,	LESS,	ASSIGN,	GREAT,	QUEST,
	UNKN,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,
	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,
	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,
	LETTER,	LETTER,	LETTER,	LBRACK,	UNKN,	RBRACK,	UNKN,	LETTER,
	UNKN,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,
	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,
	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,	LETTER,
	LETTER,	LETTER,	LETTER,	LBRACE,	OR,	RBRACE,	UNKN,	UNKN
};

/* debug function */
void printtoken(int tok, FILE *out)
{
	static char *strtab[128];
	strtab[0] = "EOFC";
	strtab[1] = "SEMI";
	strtab[2] = "LBRACE";
	strtab[3] = "RBRACE";
	strtab[4] = "LBRACK";
	strtab[5] = "RBRACK";
	strtab[6] = "LPARN";
	strtab[7] = "RPARN";
	strtab[8] = "COLON";
	strtab[9] = "COMMA";

	strtab[15] = "MCALL";
	strtab[16] = "CALL";
	strtab[17] = "DECBEF";
	strtab[18] = "INCBEF";
	strtab[19] = "DECAFT";
	strtab[20] = "INCAFT";
	strtab[21] = "EXCLA";
	strtab[22] = "NEG";
	strtab[23] = "AMPER";
	strtab[24] = "STAR";
	strtab[25] = "QUEST";

	strtab[30] = "PLUS";
	strtab[31] = "MINUS";
	strtab[32] = "MOD";
	strtab[33] = "TIMES";
	strtab[34] = "DIVIDE";
	strtab[35] = "OR";
	strtab[36] = "AND";
	strtab[37] = "LSHIFT";
	strtab[38] = "RSHIFT";
	strtab[39] = "EQUAL";
	strtab[40] = "NEQUAL";
	strtab[41] = "LESSEQ";
	strtab[42] = "LESS";
	strtab[43] = "GREATEQ";
	strtab[44] = "GREAT";

	strtab[49] = "ASSIGN";
	strtab[50] = "ASPLUS";
	strtab[51] = "ASMINUS";
	strtab[52] = "ASMOD";
	strtab[53] = "ASTIMES";
	strtab[54] = "ASDIV";
	strtab[55] = "ASOR";
	strtab[56] = "ASAND";
	strtab[57] = "ASLSH";
	strtab[58] = "ASRSH";
	strtab[59] = "ASEQUAL";
	strtab[60] = "ASNEQL";
	strtab[61] = "ASLEQ";
	strtab[62] = "ASLESS";
	strtab[63] = "ASGTQ";
	strtab[64] = "ASGREAT";

	strtab[65] = "CON";
	strtab[66] = "STRING";
	strtab[67] = "NAME";
	strtab[68] = "KEYW";

	strtab[127] = "UNKN";

	if (tok == CON || tok == STRING) {
		fprintf(out, "%s(%d) ", strtab[tok], cval);
		return;
	}
	if (tok == NAME) {
		fprintf(out, "%s(%s) ", strtab[tok], symbuf);
		return;
	}
		
	fprintf(out, "%s ", strtab[tok]);
}

