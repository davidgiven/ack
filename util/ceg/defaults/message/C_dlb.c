#define CODE_EXPANDER
#include <em.h>
#include "back.h"

extern int __db_mes, __db_len, __db_type, __db_add;
extern char *__db_str;

void
C_dlb( l, val)
label l;
arith val;
{
	if (__db_mes) {
		B_symbstr(__db_str, __db_len, __db_type, __db_add, extnd_dlb(l), val);
	}
}
