/* $Id: mach2.c, V1.4 15-Mar-89 AJM */
%token <y_word> COND
%token <y_word> LINK
%token <y_word> BRANCH
%token <y_word> DATA1
%token <y_word> DATA2
%token <y_word> DATA3
%token <y_word> SET
%token <y_word> PEE
%token <y_word> REG
%token <y_word> SHIFT
%token <y_word> RRX
%token <y_word> SDT
%token <y_word> SDT2
%token <y_word> TRANS
%token <y_word> BDT
%token <y_word> SWI
%token <y_word> ADR
%token <y_word> MUL
%token <y_word> MLA

%type <y_word> optlink optcond opts optt optp optb optexc reglist rlist
%type <y_word> optsign optpsr optshift shftcnt address offset aoptshift
%type <y_word> splitaddress splitoffset
%type <y_expr> operand 
