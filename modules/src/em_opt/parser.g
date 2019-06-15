/* Parser to read optimization patterns of the form:
		op1 op2 ... test : action
	or
		op1 op2 ... : action
   and build a program to recognize then */

%token	SFIT, UFIT, ROTATE, PSIZE, WSIZE, DWSIZE,  DEFINED, UNDEFINED, SAMESIGN;
%token	SAMEEXT, SAMENAM, OFFSET, LOGAND, LOGOR, BITAND, BITOR, XOR;
%token	MINUS, PLUS, TIMES, DIV, MOD, EQ, NE, LT, LE, GT, GE, NOT, COMP;
%token	LSHIFT, RSHIFT, COMMA, OPCODE, INT, UPLUS, UMINUS, PATARG;

%start	parser, input;

{
#ifndef NORCSID
static char rcsidp1[] = "$Id$";
#endif

#include "parser.h"
#include "alloc.h"

#define MAXPRIO 11

struct state	*states[MAXSTATES];
struct action	*actions[MAXSTATES];
struct mnems	patterns[MAXSTATES];
int		numpatterns = 0;	/* Number of patterns */
int		higheststate = 0;	/* Highest state yet allocated */
struct idf	*ops;			/* Chained list of all ops */
int		maxpattern = 0;
int		maxreplacement = 0;
int		nerrors = 0;

static int	lencurrpatt;
static int	lenthisrepl;
static int	currentstate;		/* Current state of dfa */

extern void back_token(void);
extern void parser(void);

/* Forward declarations */
void addaction(int, int, struct exp_node *,struct exp_node *, 
 struct mnem_list *);
struct mnem_elem **constructlist(struct mnem_list *, int);
struct mnem_list *addelem(struct mnem_list *, struct idf *,
	struct exp_node *);
int dotransition(int, struct idf *, struct mnem_list *, int);
int priority(int);
void LLmessage(int);

}

input	: /* empty */
	| optimization input
	;

optimization
			{
			int startline;
			struct exp_node *restrictions;
			struct exp_node *finaltest;
			struct mnem_list *repllist;
			}
	:
			{
			startline=linenum; currentstate=0;
			lencurrpatt=0; lenthisrepl=0;
			}
	patterns(&restrictions)
			{ finaltest = (struct exp_node *)NULL; }
	[
		'?'
		exp(1,&finaltest)
	]?
	':'
	action(&repllist)
			{
			numpatterns++;
			addaction(startline,currentstate,restrictions,
				finaltest,repllist);
			}
	'\n'
	;

patterns(struct exp_node **tests;)
			{
			struct mnem_list *list;
			struct exp_node *onetest;
			}
	:
			{
			list = (struct mnem_list *)NULL;
			*tests = (struct exp_node *)NULL;
			}
	[
	OPCODE
			{
			if(++lencurrpatt>maxpattern)
				maxpattern=lencurrpatt;
			list = addelem(list,opval, (struct exp_node *)NULL);
			opval->id_used=1;
			if(lencurrpatt==1)
				opval->id_startpatt=1;
			currentstate=dotransition(currentstate,opval,list,lencurrpatt);
			}
	[
		restriction(opval->id_argfmt,&onetest)
			{
			*tests = combinetests(*tests,onetest);
			}
	]?
	]+
	;

restriction(int argtype; struct exp_node **test;)
			{
			struct exp_node *test1,*test2;
			int relop;
			int offsetop;
			}
	:
	[ %if(argtype==CSTOPT)
		[ optrelop(&relop) exp(1,test)
			{
			*test = mknode(relop,mkleaf(PATARG,lencurrpatt),*test);
			}
		| DEFINED
			{
			*test = mkleaf(DEFINED,lencurrpatt);
			}
		| UNDEFINED
			{
			*test = mkleaf(UNDEFINED,lencurrpatt);
			}
		]
	| %if(argtype==EXT)
		patarg(&test1)
			{
			*test=mknode(SAMEEXT,
				mkleaf(PATARG,lencurrpatt),
				test1);
			}
		[
			[ PLUS
				{ offsetop = PLUS; }
			| MINUS
				{ offsetop = MINUS; }
			]
			exp(1,&test2)
				{
				test2 = mknode(offsetop, test1, test2);
				test2 = mknode(EQ, mkleaf(PATARG,lencurrpatt), test2);
				*test = combinetests(
					mknode(SAMENAM,
						mkleaf(PATARG,lencurrpatt),
						test1),
					test2);
				}
		]?
	|
		optrelop(&relop) exp(1,test)
			{
			*test = mknode(relop,mkleaf(PATARG,lencurrpatt),*test);
			}
	]
	;

optrelop(int *op;)
	:
		{*op = EQ;}
	[ EQ	{*op = EQ;}
	| NE	{*op = NE;}
	| LT	{*op = LT;}
	| LE	{*op = LE;}
	| GT	{*op = GT;}
	| GE	{*op = GE;}
	]?
	;
action(struct mnem_list **list;)
			{
			struct exp_node *test, *test2;
			struct idf *keepopval;
			int offsetop;
			}
	:
			{ *list = (struct mnem_list *)NULL; }
	[
	OPCODE
			{
			if(++lenthisrepl>maxreplacement)
				maxreplacement = lenthisrepl;
			test= (struct exp_node *)NULL;
			keepopval = opval;
			}
		[ %if(keepopval->id_argfmt==EXT)
		patarg(&test)
				{
				test2 = test;
				}
			[
				[ PLUS
					{ offsetop = PLUS; }
				| MINUS
					{ offsetop = MINUS; }
				]
				exp(1,&test2)
					{
					test2 = mknode(offsetop,test,test2);
					}

			]?
				{
				test = mknode(COMMA,test,test2);
				}
		| exp(1,&test)
		]?
				{
				*list = addelem(*list,keepopval,test);
				}
	]*
	;

exp(int level; struct exp_node **result;)
			{ struct exp_node *res1, *res2;
			  int operator; }
		:
		%if(level <= MAXPRIO)
		exp(MAXPRIO+1,&res1)
			[ %while (priority(LLsymb) >= level)
			  binop
				{
				operator=LLsymb;
				}
			  exp(priority(operator)+1,&res2)
				{
				res1 = mknode(operator,res1,res2);
				}
	 		]*
				{
				*result = res1;
				}
		| '(' exp(1,result) ')'
		| unaryop(&operator) exp(priority(operator)+1,&res1)
			{
			*result = mknode(operator,res1,(struct exp_node *)NULL);
			}
		| SAMESIGN '(' exp(1,&res1) COMMA exp(1,&res2) ')'
			{
			*result = mknode(SAMESIGN,res1,res2);
			}
		| SFIT '(' exp(1,&res1) COMMA exp(1,&res2) ')'
			{
			*result = mknode(SFIT,res1,res2);
			}
		| UFIT '(' exp(1,&res1) COMMA exp(1,&res2) ')'
			{
			*result = mknode(UFIT,res1,res2);
			}
		| ROTATE '(' exp(1,&res1) COMMA exp(1,&res2) ')'
			{
			*result = mknode(ROTATE,res1,res2);
			}
		| SAMEEXT '(' patarg(&res1) COMMA patarg(&res2) ')'
			{
			*result = mknode(SAMEEXT,res1,res2);
			}
		| SAMENAM '(' patarg(&res1) COMMA patarg(&res2) ')'
			{
			*result = mknode(SAMENAM,res1,res2);
			}
		| OFFSET '(' patarg(&res1) ')'
			{
			*result = res1;
			}
		| patarg(result)
		| PSIZE
			{
			*result = mkleaf(PSIZE,0);
			}
		| WSIZE
			{
			*result = mkleaf(WSIZE,0);
			}
		| DWSIZE
			{
			*result = mkleaf(DWSIZE,0);
			}
		| INT
			{
			*result = mkleaf(INT,lastintval);
			}
		;

patarg(struct exp_node **result;)
		{ int intval; }
	:  PATARG argno(&intval)
		{
		*result = mkleaf(PATARG,intval);
		}
	;

argno(int *val;)
	: INT
		{
		*val = lastintval;
		if(lastintval<0 || (lastintval>lencurrpatt)) {
			fprintf(stderr ,"Illegal $%d on line %d\n",
					lastintval,linenum);
			nerrors++;
		}
		}
	;

unaryop(int *operator;)
	: PLUS	{ *operator = UPLUS; }
	| MINUS	{ *operator = UMINUS; }
	| NOT	{ *operator = NOT; }
	| COMP	{ *operator = COMP; }
	;

binop	: LOGAND
	| LOGOR
	| BITAND
	| BITOR
	| XOR
	| MINUS
	| PLUS
	| TIMES
	| DIV
	| MOD
	| EQ
	| NE
	| LT
	| LE
	| GT
	| GE
	| LSHIFT
	| RSHIFT
	;

%lexical yylex;

{
void addaction(int startline, int state, struct exp_node *restrictions, 
	struct exp_node *finaltest, struct mnem_list *repllist)
{
	struct action *p, *q;
	p=(struct action *)Malloc(sizeof(struct action));
	p->next = (struct action *)NULL;
	p->linenum = startline;
	p->test = combinetests(restrictions,finaltest);
	p->replacement.m_len = lenthisrepl;
	p->replacement.m_elems = constructlist(repllist,lenthisrepl);
	/* chain new action to END of action chain */
	if((q = actions[state])==(struct action *)NULL)
		actions[state] = p;
	else {
		while(q->next != (struct action *)NULL)
			q = q->next;
		q->next = p;
	}
}

struct mnem_elem **constructlist(struct mnem_list *list, int len)
{
	struct mnem_elem **p;
	p = (struct mnem_elem **)
		Malloc((unsigned)(len*sizeof(struct mnem_elem *)));
	while(len--) {
		p[len] = list->elem;
		list = list->next;
	}
	return(p);
}

struct mnem_list *addelem(struct mnem_list *oldlist,
	struct idf *mnem,
	struct exp_node *test)
{
	struct mnem_list *reslist;
	struct mnem_elem *element;
	element = (struct mnem_elem *)Malloc(sizeof(struct mnem_elem));
	element->op_code = mnem;
	element->arg = test;
	reslist = (struct mnem_list *)Malloc(sizeof(struct mnem_list));
	reslist->elem = element;
	reslist->next = oldlist;
	return(reslist);
}

int dotransition(
	int state,
	struct idf *mnem,
	struct mnem_list *mnem_list,
	int lenlist)
{
	struct state *p;
	/* look for existing transition */
	for(p=states[state];
	    (p!=((struct state *)NULL)) && ((p->op)!=mnem);
	    p = p->next
	   );
	if(p==(struct state *)NULL) {
		/* none found so add a new state to dfa */
		p=(struct state *)Malloc(sizeof(struct state));
		p->op=mnem;
		if(++higheststate>MAXSTATES) {
			fprintf(stderr,"Parser: More than %d states\n",MAXSTATES);
			sys_stop(S_EXIT);
		}
		p->goto_state= higheststate;
		p->next=states[currentstate];
		states[currentstate]=p;
		states[higheststate] = (struct state *)NULL;
		actions[higheststate] = (struct action *)NULL;
		patterns[higheststate].m_len = lencurrpatt;
		patterns[higheststate].m_elems =
			constructlist(mnem_list,lenlist);
		return(higheststate);
	}
	else return(p->goto_state); /* already exists */
}

struct exp_node *
combinetests(test1, test2)
	struct exp_node *test1, *test2;
{
	if(test1==(struct exp_node *)NULL)
		return(test2);
	else if(test2==(struct exp_node *)NULL)
		return(test1);
	else
		return(mknode(LOGAND,test1,test2));
}

int priority(int op)  {
	switch (op) {
	case LOGOR:	return(1);
	case LOGAND:	return(2);
	case BITOR:	return(3);
	case XOR:	return(4);
	case BITAND:	return(5);
	case EQ:
	case NE:	return(6);
	case LT:
	case LE:
	case GT:
	case GE:	return(7);
	case LSHIFT:
	case RSHIFT:	return(8);
	case MINUS:
	case PLUS:	return(9);
	case TIMES:
	case DIV:
	case MOD:	return(10);
	case NOT:
	case COMP:
	case UPLUS:
	case UMINUS:	return(11);
	}
	fprintf(stderr,"Internal error: priority: - unrecognized operator\n");
	return(0);
}

struct exp_node *mknode(int op, struct exp_node *left,
    struct exp_node *right)
{
	struct exp_node *p;
	p = (struct exp_node *)Malloc(sizeof(struct exp_node));
	p->node_type = op;
	p->exp_left = left;
	p->exp_right = right;
	return(p);
}

struct exp_node *mkleaf(int op, int val)
{	
	struct exp_node *p;
	p = (struct exp_node *)Malloc(sizeof(struct exp_node));
	p->node_type = op;
	p->leaf_val = val;
	return(p);
}

void LLmessage(int insertedtok)
{
	nerrors++;
	fprintf(stderr,"parser: syntax error on line %d: ",linenum);
	if(insertedtok) {
		fprintf(stderr,"Inserted token %d\n",insertedtok);
		back_token();
	}
	else fprintf(stderr,"Deleted token %d\n",LLsymb);
}

int main(int argc, char **argv) 
{
	initlex();
	states[0] = (struct state *)NULL;
	patterns[0].m_len = 0;
	parser();
	if(nerrors) {
		fprintf(stderr,"%d errors detected\n",nerrors);
		sys_stop(S_EXIT);
	}
	outputnopt();
	sys_stop(S_END);
}
}
