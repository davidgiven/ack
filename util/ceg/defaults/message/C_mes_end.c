#define CODE_EXPANDER
#include <em.h>

extern int __db_mes;
extern int __db_cst;
extern char *__db_str;

C_mes_end()
{
	if (__db_mes) {
		__db_mes = 0;
		__db_cst = 0;
		__db_str = 0;
	}
}
