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
%token <y_word> RXX
%token <y_word> SDT
%token <y_word> BYTE
%token <y_word> TRANS
%token <y_word> BDT
%token <y_word> SWI
%token <y_word> ADR

%type <y_word> optlink optcond opts optt optp optb optexc reglist rlist
%type <y_word> optsign optpsr optshift shftcnt address offset
%type <y_expr> operand
