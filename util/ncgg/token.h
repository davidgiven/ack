/* $Header$ */

#ifndef _COST_
#include "cost.h"
#endif


typedef struct token {
	char	*tk_name;
	int	 tk_size;
	cost_t	 tk_cost;
	struct {
		int ta_type;	/* -1 is int, -2 is addr, >=0 is propno */
		char *ta_name;
	}	 tk_att[MAXATT];
	int	 tk_format;
} token_t,*token_p;

extern token_p l_tokens[MAXTOKENS];
