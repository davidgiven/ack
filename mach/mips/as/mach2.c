%token <y_word> GPR
%token <y_word> FPR

%type <y_word> gpr
%type <y_word> e16 e9
%type <y_word> u20 u5
%type <y_word> abs26 offset16

%type <y_word> extmsblsb insmsblsb

#include "definitions.y"

