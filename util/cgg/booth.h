#include <local.h>

#ifdef extern
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

extern char *hname INIT("tables.h");
extern char *cname INIT("tables.c");
extern char *iname;			/* stdin */

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

extern char *myalloc();

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
extern ident_p identtab[ITABSIZE];

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

extern unsigned cc1 INIT(1),cc2 INIT(1),cc3 INIT(1),cc4 INIT(1);

extern node_t  nodes[MAXNODES];
extern node_p  lastnode INIT(nodes+1);

extern string codestrings[MAXSTRINGS];
extern int ncodestrings;

extern int strar[MAXNSTR];
extern int nstr;

extern int pathash[256];

extern reginfo machregs[MAXREGS];
extern char stregclass[MAXREGS];
extern int nmachregs INIT(1);
extern int nregclasses INIT(1);
extern int maxmembers;
extern struct {
	ident_p propname;
	set_t	propset;
} machprops[MAXPROPS];
extern int nprops;
extern token_t machtokens[MAXTOKENS];
extern int nmachtokens INIT(1);
extern set_t machsets[MAXSETS];
extern int nmachsets;
extern int patmnem[MAXEMPATLEN];
extern int empatlen;
extern int maxempatlen;
extern int empatexpr;
extern int maxrule INIT(1);
extern int pattokexp[MAXPATLEN];
extern int tokpatlen;
extern int lookident;        /* lexical analyzer flag */
extern list3 structpool;
extern int nallreg;
extern int allreg[MAXALLREG];
extern int maxallreg;
extern int lino;
extern int nerrors;
extern int curtokexp;
extern expr_t arexp[TOKENSIZE];
extern int narexp;
extern inst_t arinstance[MAXINSTANCE];
extern int narinstance INIT(1);
extern move_t machmoves[NMOVES];
extern int nmoves;
extern byte pattern[MAXPATTERN];
extern int npatbytes;
extern int prevind;
extern int rulecount;                  /* Temporary index for ... construct */
extern int ncoderules;
extern int codebytes;
extern FILE *cfile;
extern FILE *hfile;
extern int maxtokensize;
extern int dealflag;
extern int emrepllen;
extern int replmnem[MAXEMPATLEN];
extern int tokrepllen;
extern int replinst[MAXPATLEN];
extern int replexpr[MAXPATLEN];
extern c1_t c1coercs[MAXC1];
extern c2_t c2coercs[MAXC2];
extern c3_t c3coercs[MAXC3];
extern int nc1,nc2,nc3;
extern int maxsplit;
extern int wsize INIT(-1);
extern int psize INIT(-1);
extern int bsize INIT(-1);
extern char *fmt;

extern int cchandled;
extern int ccspoiled;
extern int ccregexpr;
extern int ccinstanceno;
extern int cocopropno;
extern int cocosetno;
extern int allexpno;

extern int rvused;	/* regvars used */
extern int nregvar[4];	/* # of register variables of all kinds */
extern int rvnumbers[4][MAXREGVARS];	/* The register numbers */

#define chktabsiz(size,maxsize,which) if(size>=maxsize) tabovf(which)

#define MUST1BEINT(e) int exp1=e.expr_index;tstint(e)
#define MUST2BEINT(e1,e2) int exp1=e1.expr_index,exp2=e2.expr_index;tstint(e1);tstint(e2)
#define MUST1BEBOOL(e) int exp1=e.expr_index;tstbool(e)
#define MUST2BEBOOL(e1,e2) int exp1=e1.expr_index,exp2=e2.expr_index;tstbool(e1);tstbool(e2)

extern ident_p ilookup();
extern list2 lookstruct();
extern string scopy();
extern unsigned hash();
