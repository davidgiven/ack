%{
/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
# include <ctype.h>
# include "token.h"
# include "Lpars.h"

# define TAB	8	/* Size of a acsii tab (\t) in spaces */
# if (TAB&(TAB-1))!=0
# define TABSTOP(ind)	((ind)+TAB-(ind)%TAB)
# else
# define TABSTOP(ind)	(((ind)+TAB)&(~(TAB-1)))
# endif

char *Malloc(), *strcpy();

struct token token;
int ind=0;		/* Indentation level of current line */
static int tab=0;	/* First indentation found */

int included=0;		/* Is current file included? */
int lineno = 1;
%}

%%
'((\*[^\n])|([^'\n*]))*'	{
	if ((token.t_lval=char_constant(yytext+1))== -1L) 
		report("%s not a character constant", yytext);

	return CHAR_CONST;
}
'[^'\n]*'?			{
	report("missing '.");
	token.t_lval= -1L;

	return CHAR_CONST;
}
\"((\*[^\n])|([^"\n*]))*\"	{
	char *string();

	token.t_sval=string(yytext);

	return STRING;
}
\"[^"\n]*\"?			{
	report("missing \".");
	token.t_sval="";

	return STRING;
}
#[ \t]*"line"?[ \t]*[0-9]+[ \t]*\"[^"\n]*\"	{
	set_line_file(yytext);
	tab=0;
}
#[A-Fa-f0-9]+			{
	long hex_number();

	token.t_lval=hex_number(yytext+1);

	return NUMBER;
}
[0-9]+				{
	long number();

	token.t_lval=number(yytext);

	return NUMBER;
}
[A-Za-z][A-Za-z0-9.]*		{
	register key;

	if ((key=keyword(yytext))==IDENTIFIER)
		token.t_sval=strcpy(Malloc(yyleng+1), yytext);
	
	return key;
}
\n[ \f\t]*/"--"			{/* Line with only a comment, don't set tab */
				 lineno++;
				}

\n[ \f\t]*			{

	lineno++;
	ind=indentation(yytext+1);
	if (tab==0)
		tab=ind;
	else
	if (ind%tab!=0)
		warning("indentation not on a %d space boundary", tab);
}
[ \f\t]				{ /* Nothing */ }
[-=<>:,;+*/\[\]()?!&]		return yytext[0];

"\\"				return BS;
":="				return AS;
"<="				return LE;
">="				return GE;
"<>"				return NE;
"<<"				return LS;
">>"				return RS;
"/\\"				return BA;
"\\/"				return BO;
"><"				return BX;

"--"[^\n]*			{ /* Comment is skipped */ }
.				{
	warning((' '<=yytext[0] && yytext[0]<0177) ? "%s'%c')" : "%soctal: %o)",
		"bad character seen (", yytext[0]&0377);
}
%%
char *string(s) char *s;
{
	register c;
	register char *p= s;
	char *str= s;
	
	str++; p++;
	while (*str != '"') {
		if ((c=character(&str)) != -1)
			*p++= c;
		else
			return "";
	}

	*p=0;
	*s=p-(s+1);
	return s;
}

long number(s) register char *s;
{
	static char max_str[]="2147483647";
	int  maxlen=sizeof max_str-1;
	long atol();
	long num;

	while (*s=='0') { /* skip leading nulls */
		*s++;
		yyleng--;
	}

	if (*s==0)
		num=0L;
	else {
		if ((yyleng>maxlen) || (yyleng==maxlen && strcmp(s, max_str)>0))
			warning("integer constant overflow.");

		num=atol(s);
	}

	return num;
}
		
long hex_number(s) register char *s;
{
	long number=0L;

	while (*s)
		number=(number<<4)+hextoint(*s++);

	return number;
}

int hextoint(c) register c;
{
	register val;
	
	if (islower(c))
		val=(c-'a')+10;
	else
	if (isupper(c))
		val=(c-'A')+10;
	else
		val=c-'0';
	
	return val;
}

int character(S) register char **S;
{
	register char *s= *S;
	register c, cc;

	if ((c= *s++)=='*') {
		switch (c= *s++) {
		case 'c':
			cc='\r';
			break;
		case 'n':
			cc='\n';
			break;
		case 't':
			cc='\t';
			break;
		case 's':
			cc=' ';
			break;
		case '#':
			if (isxdigit(c= *s++) && isxdigit(*s)) {
				cc= (hextoint(c)<<4)+hextoint(*s++);
				break;
			} else {
				report("two digit hexadecimal const expected.");
				return -1;
			}
		default:
			cc=c;
			break;
		}
	} else
		cc=c;
	
	*S=s;
	return cc;
}
	
int char_constant(s) char *s;
{
	register cc;

	cc=character(&s);

	return (*s=='\'' && cc!= -1) ? cc : -1;
}

int indentation(s) register char *s;
{
	register in=0, c;

	while (c= *s++) {
		if (c=='\t')
			in=TABSTOP(in);
		else
		if (c=='\f')
			in=0;
		else
			in++;
	}
	
	return in;
}

int tabulated(oind, ind) register oind, ind;
{
	if (tab>0 && ind>oind+tab)
		warning("process' indentation too large (changed to %d tab%s)",
			oind/tab+1, oind>=tab ? "s" : "");
	return ind>oind;
}

int rep_tk=0;
struct token rep_token;

void repeat_token(tk)
{
	rep_tk=tk;
	rep_token=token;
}

scanner()
{
	register tk;

	if (rep_tk>0) {
		tk=rep_tk;;
		rep_tk=0;
		token=rep_token;
		return tk;
	} else
		return yylex();
}

char *tokenname(tk, inst) register tk, inst;
{
	if (tk<0400) {
		static char c[7];

		if (' '<tk && tk<='~')
			sprint(c, "'%c'", tk);
		else
			sprint(c, "'*#%02x'", tk);
		return c;
	} else {
		switch (tk) {
			char *keyname();
			char fake_id[1+sizeof(int)*3+1];
			static fake_cnt=0;
		default:
			return keyname(tk);
		case IDENTIFIER:
			if (inst) {
				sprint(fake_id, "_%d", ++fake_cnt);
				token.t_sval=strcpy(Malloc(strlen(fake_id)+1),
					fake_id);
				return "IDENTIFIER";
			} else
				return token.t_sval;
		case NUMBER:
		case CHAR_CONST:
			token.t_lval=0L;
			return "NUMBER";
		case STRING:
			if (inst) {
				token.t_sval=Malloc(1);
				token.t_sval[0]=0;
			} else
				free(token.t_sval);
			return "STRING";
		case AS:	case LE:	case GE:	case NE:
		case LS:	case RS:	case BA:	case BO:
		case BX:	case BS:	{
			static int op[]= {
				AS, LE, GE, NE, LS, RS, BA, BO, BX, BS
			};
			static char *opc[]= {
				":=", "<=", ">=", "<>", "<<", ">>", "/\\",
				"\\/", "><", "\\"
			};
			register i;
			static char qopc[5];

			for (i=0; op[i]!=tk; i++) ;
			sprint(qopc, "'%s'", opc[i]);
			return qopc;
			}
		}
	}
}

set_line_file(l) register char *l;
{
	register char *file;

	while (*l<'0' || *l>'9') l++;

	lineno=0;
	while ('0'<=*l && *l<='9')
		lineno=lineno*10+(*l++ - '0');

	lineno--;

	while (*l++!='"');

	file=l;
	while (*l++!='"');
	*--l=0;

	included=set_file(file);
}
