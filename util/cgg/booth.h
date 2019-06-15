#include <local.h>

#ifdef INIT_VAR
#define INIT(x)         = x
#else
#define INIT(x)         /* empty */
#endif

#if BIGMACHINE
#define BORS(x,y) x
#else
#define BORS(x,y) y
#endif
/* Tunable constants */

#define MAXALLREG 5             /* Maximum number of allocates per rule */
#define MAXREGS BORS(36,32)     /* Total number of registers */
#define MAXREGVARS 8		/* Maximum regvars per type */
#define MAXPROPS 16             /* Total number of register properties */
#define MAXTOKENS BORS(75,32)   /* Different kind of tokens */
#define MAXSETS BORS(100,80)    /* Number of tokenexpressions definable */
#define MAXEMPATLEN 25		/* Maximum length of EM-pattern/replacement */
#define TOKENSIZE 5             /* Maximum number of fields in token struct */
#define MAXINSTANCE BORS(250,120) /* Maximum number of different tokeninstances */
#define MAXSTRINGS BORS(800,400)/* Maximum number of different codestrings */
#define MAXPATTERN BORS(8000,6000) /* Maximum number of bytes in pattern[] */
#define MAXNODES BORS(500,400)  /* Maximum number of expression nodes */
#define MAXMEMBERS 2            /* Maximum number of subregisters per reg */
#define NMOVES BORS(50,30)      /* Maximum number of move definitions */
#define MAXC1   20              /* Maximum of coercions type 1 */
#define MAXC2   20              /* Maximum of coercions type 2 */
#define MAXC3   20              /* Maximum of coercions type 3 */
#define MAXSPLIT 4              /* Maximum degree of split */
#define MAXNSTR 40		/* Maximum consecutive strings in coderule */

EXTERN char *hname INIT("tables.h");
EXTERN char *cname INIT("tables.c");
EXTERN char *iname;			/* stdin */

/* Derived constants */

#define SETSIZE ((MAXREGS+1+MAXTOKENS+15)>>4)
#define PROPSETSIZE ((MAXPROPS+15)>>4)

#define BMASK 0377
#define BSHIFT 8

#define TRUE    1
#define FALSE   0

#define MAXPATLEN 7             /* Maximum length of tokenpatterns */

typedef char byte;
typedef char * string;

EXTERN char *myalloc();

#include <cg_pattern.h>

typedef struct list1str {
	struct list1str *l1next;
	string l1name;
} *list1;
typedef struct list2str {
	struct list2str *l2next;
	list1 l2list;
} *list2;
typedef struct list3str {
	struct list3str *l3next;
	list2 l3list;
} *list3;

typedef struct reginfo {
	string rname;
	string rrepr;
	int rsize;
	int rmembers[MAXMEMBERS];
	int rregvar;
	short rprop[PROPSETSIZE];
} *reginfo;

typedef struct tokeninfo {
	string t_name;
	list2 t_struct;
	struct {
		int t_type;
		string t_sname;
	} t_fields[TOKENSIZE-1];
	int t_size;
	cost_t t_cost;
	int t_format;
} token_t,*token_p;

typedef struct ident {
	struct ident *i_next;
	string i_name;
	int i_type;
#               define IREG 1
#               define IPRP 2
#               define ITOK 3
#               define IEXP 4
	union {
		int i_regno;
		int i_prpno;
		int i_tokno;
		int i_expno;
	} i_i;
} ident_t,*ident_p;

#define ITABSIZE 32
EXTERN ident_p identtab[ITABSIZE];

#define LOOKUP          0
#define HALFWAY         1
#define ENTER           2
#define JUSTLOOKING     3


typedef struct expr {
	int expr_typ;
#               define TYPINT  1
#               define TYPREG  2
#               define TYPSTR  3
#               define TYPBOOL 4
	int expr_index;
} expr_t,*expr_p;

EXTERN unsigned cc1 INIT(1),cc2 INIT(1),cc3 INIT(1),cc4 INIT(1);

EXTERN node_t  nodes[MAXNODES];
EXTERN node_p  lastnode INIT(nodes+1);

EXTERN string codestrings[MAXSTRINGS];
EXTERN int ncodestrings;

EXTERN int strar[MAXNSTR];
EXTERN int nstr;

EXTERN int pathash[256];

EXTERN reginfo machregs[MAXREGS];
EXTERN char stregclass[MAXREGS];
EXTERN int nmachregs INIT(1);
EXTERN int nregclasses INIT(1);
EXTERN int maxmembers;
EXTERN struct {
	ident_p propname;
	set_t	propset;
} machprops[MAXPROPS];
EXTERN int nprops;
EXTERN token_t machtokens[MAXTOKENS];
EXTERN int nmachtokens INIT(1);
EXTERN set_t machsets[MAXSETS];
EXTERN int nmachsets;
EXTERN int patmnem[MAXEMPATLEN];
EXTERN int empatlen;
EXTERN int maxempatlen;
EXTERN int empatexpr;
EXTERN int maxrule INIT(1);
EXTERN int pattokexp[MAXPATLEN];
EXTERN int tokpatlen;
EXTERN int lookident;        /* lexical analyzer flag */
EXTERN list3 structpool;
EXTERN int nallreg;
EXTERN int allreg[MAXALLREG];
EXTERN int maxallreg;
EXTERN int lino;
EXTERN int nerrors;
EXTERN int curtokexp;
EXTERN expr_t arexp[TOKENSIZE];
EXTERN int narexp;
EXTERN inst_t arinstance[MAXINSTANCE];
EXTERN int narinstance INIT(1);
EXTERN move_t machmoves[NMOVES];
EXTERN int nmoves;
EXTERN byte pattern[MAXPATTERN];
EXTERN int npatbytes;
EXTERN int prevind;
EXTERN int rulecount;                  /* Temporary index for ... construct */
EXTERN int ncoderules;
EXTERN int codebytes;
EXTERN FILE *cfile;
EXTERN FILE *hfile;
EXTERN int maxtokensize;
EXTERN int dealflag;
EXTERN int emrepllen;
EXTERN int replmnem[MAXEMPATLEN];
EXTERN int tokrepllen;
EXTERN int replinst[MAXPATLEN];
EXTERN int replexpr[MAXPATLEN];
EXTERN c1_t c1coercs[MAXC1];
EXTERN c2_t c2coercs[MAXC2];
EXTERN c3_t c3coercs[MAXC3];
EXTERN int nc1,nc2,nc3;
EXTERN int maxsplit;
EXTERN int wsize INIT(-1);
EXTERN int psize INIT(-1);
EXTERN int bsize INIT(-1);
EXTERN char *fmt;

EXTERN int cchandled;
EXTERN int ccspoiled;
EXTERN int ccregexpr;
EXTERN int ccinstanceno;
EXTERN int cocopropno;
EXTERN int cocosetno;
EXTERN int allexpno;

EXTERN int rvused;	/* regvars used */
EXTERN int nregvar[4];	/* # of register variables of all kinds */
EXTERN int rvnumbers[4][MAXREGVARS];	/* The register numbers */

#define chktabsiz(size,maxsize,which) if(size>=maxsize) tabovf(which)

#define MUST1BEINT(e) int exp1=e.expr_index;tstint(e)
#define MUST2BEINT(e1,e2) int exp1=e1.expr_index,exp2=e2.expr_index;tstint(e1);tstint(e2)
#define MUST1BEBOOL(e) int exp1=e.expr_index;tstbool(e)
#define MUST2BEBOOL(e1,e2) int exp1=e1.expr_index,exp2=e2.expr_index;tstbool(e1);tstbool(e2)

ident_p ilookup(string name, int enterf);
unsigned hash(register string name);
int lookup(int comm, int operator, int lnode, int rnode);
list2 lookstruct(list2 ll);
string scopy(string s);
void tabovf(string tablename);
int strlookup(string s);
int structsize(register list2 s);
void yyerror(string s, ...);
int instno(inst_t inst);
int yyparse(void);
int stringno(register string s);
int exprlookup(set_t sett);
void tstint(expr_t e);
void tstbool(expr_t e);
void inbetween(void);
int formconversion(register char *p, register token_p tp);
void setfields(register token_p tp, string format);
void chkregexp(int number);
int findstructel(int number, string name, int *t);
int argtyp(int mn);
int commontype(expr_t e1, expr_t e2);
void inithash(void);
void enter(char *name, int value);
int mlookup(char *name);
void hashpatterns(void);
int max(int a, int b);
void out(int n);
void outbyte(int n);
void pat(int n);
void patshort(int n);
void patbyte(int n);

