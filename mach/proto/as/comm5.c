/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* @(#)comm5.c	1.1 */

#include "comm0.h"
#include "comm1.h"
#include "y.tab.h"

extern YYSTYPE yylval;

static void readcode(int);
static int induo(int);
static int inident(int);
static int innumber(int);
static int instring(int);
static int inescape(void);
static int infbsym(const char*);

static int maxstring = 0;

int yylex(void)
{
	int c, c0, c1;

	if (pass == PASS_1)
	{
		/* scan the input file */
		do
			c = nextchar();
		while (isspace(c) && c != '\n');
		if (ISALPHA(c))
			c = inident(c);
		else if (isdigit(c))
			c = innumber(c);
		else
			switch (c)
			{
				case '=':
				case '<':
				case '>':
				case '|':
				case '&':
					c = induo(c);
					break;
				case ASC_SQUO:
				case ASC_DQUO:
					c = instring(c);
					break;
				case ASC_COMM:
					do
						c = nextchar();
					while (c != '\n' && c != '\0');
					break;
				case CTRL('A'):
					c = CODE1;
					readcode(1);
					break;
				case CTRL('B'):
					c = CODE2;
					readcode(2);
					break;
				case CTRL('C'):
					c = CODE4;
					readcode(4);
					break;
			}

		/* produce the intermediate token file */
		if (c <= 0)
			return (0);
		if (c < 256)
		{
			putc(c, tempfile);
			putc(0, tempfile);
		}
		else
		{
			putval(c);
		}
	}
	else
	{
		/* read from intermediate token file */
		c0 = getc(tempfile);
		if (c0 == EOF)
			return (0);
		c1 = getc(tempfile);
		if (c1 == EOF)
			return (0);

		c = c0 + (c1 << 8);
		if (c >= 256)
			c = getval(c);
	}
	curr_token = c;
	return (c);
}

void putval(int c)
{
	int64_t v;
	int n = 0;
	char* p = 0;

	assert(c == (c & 0xffff));
	switch (c)
	{
		case CODE1:
			n = 1;
			v = yylval.y_valu;
			goto putnum;
		case CODE2:
			n = 2;
			v = yylval.y_valu;
			goto putnum;
		case CODE4:
			n = 4;
			v = yylval.y_valu;
			goto putnum;
		case NUMBER8:
			v = yylval.y_valu;
			for (n = 0; n < sizeof(v); n++)
			{
				if (v == 0)
					break;
				v >>= 8;
			}
			if (n <= 4)
				c = NUMBER0 + n;
			else
				n = 8;
			v = yylval.y_valu;
		putnum:
			putc(c, tempfile);
			putc(c >> 8, tempfile);
			while (--n >= 0)
				putc((int)(v >> (n * 8)), tempfile);
			return;
		case IDENT:
		case FBSYM:
			n = sizeof(item_t*);
			p = (char*)&yylval.y_item;
			break;
#ifdef ASLD
		case MODULE:
			n = sizeof(char*);
			p = (char*)&yylval.y_strp;
			break;
#endif
		case STRING:
		case NUMBERF:
			v = stringlen;
			putc(c, tempfile);
			putc(c >> 8, tempfile);
			for (n = 0; n < sizeof(v); n++)
			{
				if (v == 0)
					break;
				v >>= 8;
			}
			assert(n <= 4);
			putc(n, tempfile);
			v = stringlen;
			while (--n >= 0)
				putc((int)(v >> (n * 8)), tempfile);
			p = stringbuf;
			n = stringlen;
			while (--n >= 0)
				putc(*p++, tempfile);
			return;
		case OP_EQ:
		case OP_NE:
		case OP_LE:
		case OP_GE:
		case OP_LL:
		case OP_RR:
		case OP_OO:
		case OP_AA:
			break;
		default:
			n = sizeof(word_t);
			p = (char*)&yylval.y_word;
			break;
	}
	putc(c, tempfile);
	putc(c >> 8, tempfile);
	while (--n >= 0)
		putc(*p++, tempfile);
}

int getval(int c)
{
	int64_t v;
	int n = 0;
	char* p = 0;

	switch (c)
	{
		case CODE1:
			n = 1;
			goto getnum;
		case CODE2:
			n = 2;
			goto getnum;
		case CODE4:
			n = 4;
			goto getnum;
		case NUMBER0:
			c = NUMBER8;
			goto getnum;
		case NUMBER1:
			n = 1;
			c = NUMBER8;
			goto getnum;
		case NUMBER2:
			n = 2;
			c = NUMBER8;
			goto getnum;
		case NUMBER3:
			n = 3;
			c = NUMBER8;
			goto getnum;
		case NUMBER4:
			n = 4;
			c = NUMBER8;
			goto getnum;
		case NUMBER8:
			n = 8;
		getnum:
			v = 0;
			while (--n >= 0)
			{
				v <<= 8;
				v |= getc(tempfile);
			}
			yylval.y_valu = v;
			return (c);
		case IDENT:
		case FBSYM:
			n = sizeof(item_t*);
			p = (char*)&yylval.y_item;
			break;
#ifdef ASLD
		case MODULE:
			n = sizeof(char*);
			p = (char*)&yylval.y_strp;
			break;
#endif
		case STRING:
		case NUMBERF:
			getval(getc(tempfile) + NUMBER0);
			stringlen = n = yylval.y_valu;
			p = stringbuf;
			p[n] = '\0';
			break;
		case OP_EQ:
		case OP_NE:
		case OP_LE:
		case OP_GE:
		case OP_LL:
		case OP_RR:
		case OP_OO:
		case OP_AA:
			break;
		default:
			n = sizeof(word_t);
			p = (char*)&yylval.y_word;
			break;
	}
	while (--n >= 0)
		*p++ = getc(tempfile);
	return (c);
}

/* ---------- lexical scan in pass 1 ---------- */

int nextchar(void)
{
	int c;

	if (peekc != -1)
	{
		c = peekc;
		peekc = -1;
		return (c);
	}
#ifdef ASLD
	if (archmode && --archsize < 0)
		return (0);
#endif
	if ((c = getc(input)) == EOF)
		return (0);
	if (isascii(c) == 0)
		fatal("non-ascii character");
#ifdef LISTING
	if (listflag & 0440)
		putc(c, listfile);
#endif
	return (c);
}

static void readcode(int n)
{
	int c;

	yylval.y_valu = 0;
	do
	{
		if (
#ifdef ASLD
		    (archmode && --archsize < 0) ||
#endif
		    (c = getc(input)) == EOF)
			fatal("unexpected EOF in compact input");
		yylval.y_valu <<= 8;
		yylval.y_valu |= c;
	} while (--n);
}

static int induo(int c)
{
	static short duo[] = {
		('=' << 8) | '=',
		OP_EQ,
		('<' << 8) | '>',
		OP_NE,
		('<' << 8) | '=',
		OP_LE,
		('>' << 8) | '=',
		OP_GE,
		('<' << 8) | '<',
		OP_LL,
		('>' << 8) | '>',
		OP_RR,
		('|' << 8) | '|',
		OP_OO,
		('&' << 8) | '&',
		OP_AA,
		0 /* terminates array */
	};
	short* p;

	c = (c << 8) | nextchar();
	for (p = duo; *p; p++)
		if (*p++ == c)
			return (*p++);
	peekc = c & 0377;
	return (c >> 8);
}

static char name[NAMEMAX + 1];

static int inident(int c)
{
	char* p = name;
	item_t* ip;
	int n = NAMEMAX;

	do
	{
		if (--n >= 0)
			*p++ = c;
		c = nextchar();
	} while (ISALNUM(c));
	*p = '\0';
	peekc = c;
	ip = item_search(name);
	if (ip == 0)
	{
		ip = item_alloc(S_UND);
		ip->i_name = remember(name);
		/* printf("ident %s %o\n", ip->i_name, ip); */
		unresolved++;
		item_insert(ip, H_LOCAL + (hashindex % H_SIZE));
	}
	else if (hashindex < H_SIZE)
	{
		assert(H_KEY == 0);
		yylval.y_word = (word_t)ip->i_valu;
		return (ip->i_type);
	}
	yylval.y_item = ip;
	return (IDENT);
}

#ifdef ASLD
char* readident(int c)
{
	int n = NAMEMAX;
	char* p = name;

	do
	{
		if (--n >= 0)
			*p++ = c;
		c = nextchar();
	} while (ISALNUM(c));
	*p++ = '\0';
	peekc = c;
	return (name);
}
#endif

static void need_stringbuf()
{
	if (!maxstring)
	{
		maxstring = STRINGMAX;
		if ((stringbuf = malloc(maxstring)) == 0)
		{
			fatal("out of memory");
		}
	}
}

static int innumber(int c)
{
	uvalu_t uv;
	char* p;
	int radix;
	static char num[40 + 1];

	p = num;
	radix = 40;
	if (c == '.')
		goto floatconstant;
	do
	{
		if (--radix < 0)
			fatal("number too long");
		if (isupper(c))
			c += ('a' - 'A');
		*p++ = c;
		c = nextchar();
		if (c == '.')
			goto floatconstant;
	} while (isalnum(c));
	peekc = c;
	*p = '\0';
	c = *--p;
	p = num;
	radix = 10;
	if (*p == '0')
	{
		radix = 8;
		p++;
		if (*p == 'x')
		{
			radix = 16;
			p++;
		}
		else if (*p == 'b')
		{
			radix = 2;
			p++;
		}
	}
	if (radix != 16 && (c == 'f' || c == 'b'))
		return (infbsym(num));
	uv = 0;
	while ((c = *p++))
	{
		if (c > '9')
			c -= ('a' - '9' - 1);
		c -= '0';
		if ((unsigned)c >= radix)
			serror("digit exceeds radix");
		uv = uv * radix + c;
	}
	yylval.y_valu = uv; /* signed = unsigned */
	return (NUMBER8);

floatconstant:
	do
	{
		if (--radix < 0)
			fatal("number too long");
		*p++ = c;
		c = nextchar();
		if (isupper(c))
			c = tolower(c);
	} while (isdigit(c) || (c == '.') || (c == 'e') || (c == '+') || (c == '-'));
	peekc = c;

	*p = '\0';
	stringlen = p - num;
	need_stringbuf();
	assert(stringlen < maxstring);
	strcpy(stringbuf, num);
	return NUMBERF;
}

static int instring(int termc)
{
	char* p;
	int c;

	need_stringbuf();
	p = stringbuf;
	for (;;)
	{
		c = nextchar();
		if (c == '\n' || c == '\0')
		{
			peekc = c;
			serror("non-terminated string");
			break;
		}
		if (c == termc)
			break;
		if (c == '\\')
			c = inescape();
		if (p >= &stringbuf[maxstring - 1])
		{
			int cnt = p - stringbuf;

			if ((stringbuf = realloc(stringbuf, maxstring += 256)) == 0)
			{
				fatal("out of memory");
			}
			p = stringbuf + cnt;
		}
		*p++ = c;
	}
	stringlen = p - stringbuf;
	*p = '\0';
	return (STRING);
}

static int inescape(void)
{
	int c, j, r;

	c = nextchar();
	if (c >= '0' && c <= '7')
	{
		r = c - '0';
		for (j = 0; j < 2; j++)
		{
			c = nextchar();
			if (c < '0' || c > '7')
			{
				peekc = c;
				return (r);
			}
			r <<= 3;
			r += (c - '0');
		}
		return (r);
	}
	switch (c)
	{
		case 'b':
			return ('\b');
		case 'f':
			return ('\f');
		case 'n':
			return ('\n');
		case 'r':
			return ('\r');
		case 't':
			return ('\t');
		case '\'':
			return ('\'');
		case '"':
			return ('"');
	}
	return (c);
}

static int infbsym(const char* p)
{
	int lab;
	item_t* ip;

	lab = *p++ - '0';
	if ((unsigned)lab < 10)
	{
		if (*p++ == 'f')
		{
			ip = fb_ptr[FB_FORW + lab];
			if (ip == 0)
			{
				ip = fb_alloc(lab);
				fb_ptr[FB_FORW + lab] = ip;
			}
			goto ok;
		}
		ip = fb_ptr[FB_BACK + lab];
		if (ip != 0 && *p == 0)
			goto ok;
	}
	serror("bad numeric label");
	ip = fb_alloc(0);
ok:
	yylval.y_item = ip;
	return (FBSYM);
}

int hash(const char* p)
{
	unsigned short h;
	int c;

	h = 0;
	while ((c = *p++))
	{
		h <<= 2;
		h += c;
	}
	return (h % H_SIZE);
}

item_t* item_search(const char* p)
{
	int h;
	item_t* ip;

	for (h = hash(p); h < H_TOTAL; h += H_SIZE)
	{
		ip = hashtab[h];
		while (ip != 0)
		{
			if (strcmp(p, ip->i_name) == 0)
				goto done;
			ip = ip->i_next;
		}
	}
done:
	hashindex = h;
	return (ip);
}

void item_insert(item_t* ip, int h)
{
	ip->i_next = hashtab[h];
	hashtab[h] = ip;
}

item_t* item_alloc(int typ)
{
	item_t* ip;
	static int nleft = 0;
	static item_t* next;

	if (--nleft < 0)
	{
		next = (item_t*)malloc(MEMINCR);
		if (next == 0)
			fatal("out of memory");
		nleft += (MEMINCR / sizeof(item_t));
	}
	ip = next++;
	ip->i_next = 0;
	ip->i_type = typ;
	ip->i_name = 0;
	ip->i_valu = 0;
	return (ip);
}

item_t* fb_alloc(int lab)
{
	item_t *ip, *p;

	ip = item_alloc(S_UND);
	p = fb_ptr[FB_TAIL + lab];
	if (p == 0)
		fb_ptr[FB_HEAD + lab] = ip;
	else
		p->i_next = ip;
	fb_ptr[FB_TAIL + lab] = ip;
	return (ip);
}

item_t* fb_shift(int lab)
{
	item_t* ip;

	ip = fb_ptr[FB_FORW + lab];
	if (ip == 0)
	{
		if (pass == PASS_1)
		{
			ip = fb_alloc(lab);
		}
		else
		{
			ip = fb_ptr[FB_HEAD + lab];
		}
	}
	fb_ptr[FB_BACK + lab] = ip;
	fb_ptr[FB_FORW + lab] = ip->i_next;
	return (ip);
}
