#define CODE_EXPANDER
#include <em.h>
#include "back.h"

extern int __db_mes, __db_len, __db_type, __db_add;
extern char *__db_str;

void
C_pnam( str)
char *str;
{
	if (__db_mes) {
		B_symbstr(__db_str, __db_len, __db_type, __db_add, extnd_name(str), (arith) 0);
	}
}
