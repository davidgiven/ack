#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define	NCPS	8	/* chars per symbol */
#define	NCPW	4	/* chars per word */
#define	ALIGN	4	/* Passed directly to the assembler's .align */
#define	HSHSIZ	400	/* hash table size */
#define	SWSIZ	230	/* switch table size */
#define	CMSIZ	40	/* symbol stack size */
#define	SSIZE	20	/* operator and precedence stack size */
#define	OSSIZ	300*8	/* space for expression tree */

#define	EOS	04	/* end of string marker */

/*
 * Holds a B symbol.
 * class is one of the storage classes below.
 * offset is used depending on class.
 */
struct	hshtab {
	int	class;
	int	offset;
	int 	dim;
	struct	hshtab *next;
	char	name[NCPS+1];
};

struct	tnode {
	int	op;
	int 	value;
	struct tnode *tr1;
	struct tnode *tr2;
};

struct	swtab {
	int	swlab;
	int	swval;
};

struct	hshtab hshtab[HSHSIZ];
int	hshused;
int	eof;
int	peekc;
char	ctab[128];
struct	hshtab *bsym;
struct	hshtab *paraml, *parame;
int	cval;
int	isn;
char	symbuf[NCPS+1];
FILE	*sbufp;
int	stack;
struct	tnode **cp;
int	*space;
int	ospace[OSSIZ];
int	retlab;
int	nerror;
struct	swtab swtab[SWSIZ];
struct	swtab *swp;
int	deflab;
extern int	contlab;
extern int	brklab;

extern int opdope[];
extern int line;
extern int peeksym, peeksym2;

void error(char *s, ...);
void printtoken(int tok, FILE *out);
struct tnode * block(int op, int value, struct tnode *tr1, struct tnode *tr2);
void rcexpr(struct tnode *tr);
void cbranch(struct tnode *t, int lab, int val);
void jump(int lab);
void label(int l);

#define	EOFC	0
#define	SEMI	1
#define	LBRACE	2
#define	RBRACE	3
#define	LBRACK	4
#define	RBRACK	5
#define	LPARN	6
#define	RPARN	7
#define	COLON	8
#define	COMMA	9

#define	MCALL	15
#define	CALL	16
#define	DECBEF	17
#define	INCBEF	18
#define	DECAFT	19
#define	INCAFT	20
#define	EXCLA	21
#define	NEG	22
#define	AMPER	23
#define	STAR	24
#define	QUEST	25

#define	PLUS	30
#define	MINUS	31
#define	MOD	32
#define	TIMES	33
#define	DIVIDE	34
#define	OR	35
#define	AND	36
#define	LSHIFT	37
#define	RSHIFT	38
#define	EQUAL	39
#define	NEQUAL	40
#define	LESSEQ	41
#define	LESS	42
#define	GREATEQ	43
#define	GREAT	44

#define	ASSIGN	49
#define	ASPLUS	50
#define	ASMINUS	51
#define	ASMOD	52
#define	ASTIMES	53
#define	ASDIV	54
#define	ASOR	55
#define	ASAND	56
#define	ASLSH	57
#define	ASRSH	58
#define	ASEQUAL	59
#define	ASNEQL	60
#define	ASLEQ	61
#define	ASLESS	62
#define	ASGTQ	63
#define	ASGREAT	64

#define	CON	65
#define	STRING	66
#define	NAME	67
#define	KEYW	68

#define	SQUOTE	121
#define	DQUOTE	122
#define	NEWLN	123
#define	SPACE	124
#define	LETTER	125
#define	DIGIT	126
#define	UNKN	127

#define	SEOF	200

/* storage classes */
#define AUTO	1
#define EXTERN	2
#define INTERN	3
#define ARG	4
#define KEYWF	5

/* keywords */
#define	CASE	3
#define	IF	4
#define	ELSE	5
#define	WHILE	6
#define	SWITCH	7
#define	GOTO	8
#define	RETURN	9
#define	DEFAULT	10
#define	BREAK	11

