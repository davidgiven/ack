#define CODE_EXPANDER
#include <em.h>
#include <em_mes.h>

int __db_mes;
char *__db_str = 0;
int __db_len = 0;
int __db_cst = 0;
int __db_type;
int __db_add;

C_mes_begin( ms)
int ms;
{
	__db_mes = (ms == ms_stb || ms == ms_std) ? ms : 0;
}
