#include <stdio.h>
#include <string.h>
#include "flt_arith.h"

struct tests {
	char	*op1, *op2;
	int	oper;
	char	*result;
} tests[] = {
  	{ "1.0", 0, 0, "1.0" },
	{ "-1.0", 0, 0, "-1.0" },
	{ "0.0", 0, 0, "0.0" },
	{ "1.234567", 0, 0, "1.234567" },
	{ "1.234567", 0, 'D', "1.0" },
	{ "1.234567", 0, 'R', "2.34567e-1" },
	{ "32768", "32768", '+', "6.5536e+4" },
	{ "32768", "32767", '-', "1.0" },
	{ "32768", "32768", '*', "1.073741824e+9" },
	{ "32768", "32768", '/', "1.0" },
	{ "1.234567e20", "-1.234567e20", '+', "0.0" },
	{ "1e100", "1e100", '+', "2.0e+100" },
	{ "1e110", "10", '*', "1.0e+111" },
	{ "0.5e100", "0.5e100", '*', "2.5e+199" },
	{ 0, 0, 0, 0}
};

int
main()
{
	register struct tests *p = tests;
	int exit_status = 0;

	while (p->op1) {
		if (! dotest(p)) exit_status = 1;
		p++;
	}
	return exit_status;
}

int
dotest(p)
	register struct tests *p;
{
	char buf[128];
	flt_arith e1, e2, e;
	static int testno = 0;

	testno++;
	flt_str2flt(p->op1, &e1);
	switch(p->oper) {
	case '+':
		flt_str2flt(p->op2, &e2);
		flt_add(&e1, &e2, &e);
		break;
	case '-':
		flt_str2flt(p->op2, &e2);
		flt_sub(&e1, &e2, &e);
		break;
	case '*':
		flt_str2flt(p->op2, &e2);
		flt_mul(&e1, &e2, &e);
		break;
	case '/':
		flt_str2flt(p->op2, &e2);
		flt_div(&e1, &e2, &e);
		break;
	case 'D':
		/* integer part of flt_modf */
		flt_modf(&e1, &e, &e2);
		break;
	case 'R':
		/* fraction part of flt_modf */
		flt_modf(&e1, &e2, &e);
		break;
	case 'U':
		/* unary minus */
		e = e1;
		flt_umin(&e);
		break;
	default:
		e = e1;
		break;
	}
	flt_flt2str(&e, buf, 128);

	if (! strcmp(buf, p->result)) return 1;

	printf("Test number %d failed: result = %s, should be %s\n", testno, buf, p->result);
	return 0;
}
