#define CODE_EXPANDER
#include <em.h>
#include <stb.h>
#include <em_mes.h>

extern int __db_mes, __db_cst, __db_len, __db_type, __db_add;
extern char *__db_str;

C_cst( l)
arith l;
{
	if (__db_mes) {
		if (! __db_cst) __db_type = l;
		else if (__db_cst == 1) {
			if (__db_mes == ms_std) {
				if (__db_type == N_SLINE) {
					C_db_sline();
				}
				B_symd(__db_str, __db_len, __db_type, (int) l);
			}
			else __db_add = l;
		}
		else B_symbcst(__db_str, __db_len, __db_type, __db_add, l);
		__db_cst++;
	}
}
