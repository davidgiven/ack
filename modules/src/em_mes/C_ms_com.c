/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
 
#include <stdlib.h>
#include <string.h>
#include "em.h"
#include "em_mes.h"

void C_ms_com(char* str)
{
	C_mes_begin(ms_com);
	C_scon(str, (arith) (strlen(str) + 1));
	C_mes_end();
}
