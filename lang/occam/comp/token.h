/*	token.h		*/

extern struct token {
	long	t_lval;
	char	*t_sval;
} token;

extern ind;
void repeat_token();
char *tokenname();
int tabulated();
