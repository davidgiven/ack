#define CODE_EXPANDER
#include <em.h>

extern int __db_mes;
extern char *__db_str;
extern int __db_len;

C_scon( str, siz)
char *str;
arith siz;
{
	if (__db_mes) {
		__db_str = str;
		__db_len = siz;
	}
}
