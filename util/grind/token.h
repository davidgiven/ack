/* $Id$ */

/* token structure for lexical analyzer */

struct token {
  int	tokno;
  long	ival;
  char	*str;
  double fval;
  struct idf *idf;
};

extern struct token	tok, aside;

#define TOK	tok.tokno
#define ASIDE	aside.tokno
