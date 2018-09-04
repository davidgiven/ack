%token <y_word> GPR
%token <y_word> FPR
%token <y_word> FCOND

%token <y_word> OP_LI
%token <y_word> OP_LA

%type <y_word> gpr fpr
%type <y_word> e16 e9
%type <y_word> u25 u20 u16 u5 u3
%type <y_word> abs26 offset16

%type <y_word> fmt fmt3
%type <y_word> fcond

%type <y_word> extmsblsb insmsblsb

#include "definitions.y"

