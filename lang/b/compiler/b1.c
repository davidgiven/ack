#include "b.h"

/*
 * Code generation (EM)
 */

static int
shiftsize(void)
{
	switch (wordsize) {
		case 1: return 0;
		case 2: return 1;
		case 4: return 2;
		case 8: return 3;
		default:
			error("unsupported word size");
			exit(1);
	}
}

void
tonativeaddr(void)
{
	C_loc(shiftsize());
	C_slu(wordsize);
}

void
fromnativeaddr(void)
{
	C_loc(shiftsize());
	C_sru(wordsize);
}

char*
manglename(char* name, char prefix)
{
	static char buffer[NCPS+3];
	buffer[0] = prefix;
	buffer[1] = '_';
	strcpy(buffer+2, name);
	return buffer;
}

void
binary(struct tnode *tr)
{
	rcexpr(tr->tr1);
	rcexpr(tr->tr2);
}

int
pushargs(struct tnode *tr)
{
	int stk;

	if (tr == NULL)
		return 0;
	if (tr->op == COMMA) {
		rcexpr(tr->tr2);
		stk = pushargs(tr->tr1);
		return stk+wordsize;
	}
	rcexpr(tr);
	return wordsize;
}

void
lvalexp(struct tnode *tr)
{
	struct hshtab *bs;
	char memloc[64];

	switch (tr->op) {

	case DECBEF:
	case INCBEF:
	case DECAFT:
	case INCAFT:
		if (tr->tr1->op == STAR) {
			rcexpr(tr->tr1->tr1);
			tonativeaddr();
		} else {	/* NAME, checked in "build" */
			bs = (struct hshtab *) tr->tr1->tr1;
			if (bs->class == EXTERN)
				C_lae_dnam(manglename(bs->name, 'b'), 0);
			else if (bs->class == AUTO)
				C_lal(bs->offset);
			else
				goto classerror;
		}
		if (tr->op == DECBEF || tr->op == INCBEF) {
			C_dup(wordsize); /* ( addr addr -- ) */
			C_loi(wordsize); /* ( addr val -- ) */
			C_adp((tr->op == DECBEF) ? -1 : 1); /* ( addr newval -- ) */
			C_exg(wordsize); /* ( newval addr -- ) */
			C_dup(wordsize*2); /* ( newval addr newval addr -- ) */
			C_sti(wordsize); /* ( newval addr -- ) */
			C_asp(wordsize); /* ( newval -- ) */
		} else {
			C_dup(wordsize); /* ( addr addr -- ) */
			C_loi(wordsize); /* ( addr val -- ) */
			C_exg(wordsize); /* ( val addr -- ) */
			C_dup(wordsize*2); /* ( val addr val addr -- ) */
			C_asp(wordsize); /* ( val addr val -- ) */
			C_adp((tr->op == DECAFT) ? -1 : 1); /* ( val addr newval -- ) */
			C_exg(wordsize); /* ( val newval addr -- ) */
			C_sti(wordsize); /* ( val -- ) */
		}
		return;

	case ASSIGN:
		rcexpr(tr->tr2);
		C_dup(wordsize);
		if (tr->tr1->op == STAR) {
			rcexpr(tr->tr1->tr1);
			tonativeaddr();
			C_sti(wordsize);
		} else {	/* NAME */
			bs = (struct hshtab *) tr->tr1->tr1;
			if (bs->class == EXTERN) {
				C_ste_dnam(bs->name, 0);
			} else if (bs->class == AUTO) {
				C_stl(bs->offset);
			} else
				goto classerror;
		}
		return;

	case ASPLUS:
	case ASMINUS:
	case ASMOD:
	case ASTIMES:
	case ASDIV:
	case ASOR:
	case ASAND:
	case ASLSH:
	case ASRSH:
	case ASEQUAL:
	case ASNEQL:
	case ASLEQ:
	case ASLESS:
	case ASGTQ:
	case ASGREAT:
	case ASEOR:
		tr->op -= ASPLUS-PLUS;
		rcexpr(block(ASSIGN,0,tr->tr1,tr));
		return;
	}

classerror:
	error("Storage class");
}

void
rcexpr(struct tnode *tr)
{
	int o1, o2;
	int stk;
	struct hshtab *bs;

	if (tr == NULL)
		return;

	if (opdope[tr->op]&02) {
		lvalexp(tr);
		return;
	}

	switch (tr->op) {

	case CON:
		C_loc(tr->value);
		return;

	case STRING:
		C_lae_dlb(tr->value, 0);
		fromnativeaddr();
		return;

	case NAME:	/* only rvalue */
		bs = (struct hshtab *) tr->tr1;
		if (bs->class == EXTERN)
			C_loe_dnam(manglename(bs->name, 'b'), 0);
		else if (bs->class == AUTO)
			C_lol(bs->offset);
		else
			goto classerror;
		return;

	case CALL:
		stk = pushargs(tr->tr2);
		rcexpr(tr->tr1);
		tonativeaddr();
		C_cai();
		if (stk)
			C_asp(stk);
		C_lfr(wordsize);
		return;

	case AMPER:
		bs = (struct hshtab *) tr->tr1->tr1;
		if (bs->class == EXTERN) {
			C_lae_dnam(manglename(bs->name, 'b'), 0);
		} else if (bs->class == AUTO) {
			C_lal(bs->offset);
		} else
			goto classerror;
		fromnativeaddr();
		return;

	case STAR:	/* only rvalue */
		rcexpr(tr->tr1);
		tonativeaddr();
		C_loi(wordsize);
		return;

	case PLUS:
		binary(tr);
		C_adi(wordsize);
		return;

	case MINUS:
		binary(tr);
		C_sbi(wordsize);
		return;

	case TIMES:
		binary(tr);
		C_mli(wordsize);
		return;

	case DIVIDE:
		binary(tr);
		C_dvi(wordsize);
		return;

	case MOD:
		binary(tr);
		C_rmi(wordsize);
		return;

	case AND:
		binary(tr);
		C_and(wordsize);
		return;

	case OR:
		binary(tr);
		C_ior(wordsize);
		return;

	case EOR:
		binary(tr);
		C_xor(wordsize);
		return;

	case LSHIFT:
		binary(tr);
		C_sli(wordsize);
		return;

	case RSHIFT:
		binary(tr);
		C_sri(wordsize);
		return;

	case EQUAL:
	case NEQUAL:
	case LESS:
	case LESSEQ:
	case GREAT:
	case GREATEQ:
		binary(tr);
		C_cmi(wordsize);
		switch (tr->op) {
		case EQUAL:
			C_teq();
			break;
		case NEQUAL:
			C_tne();
			break;
		case LESS:
			C_tlt();
			break;
		case LESSEQ:
			C_tle();
			break;
		case GREAT:
			C_tgt();
			break;
		case GREATEQ:
			C_tge();
			break;
		}
		return;

	case EXCLA:
		rcexpr(tr->tr1);
		C_teq();
		return;

	case NEG:
		rcexpr(tr->tr1);
		C_ngi(wordsize);
		return;

	case NOT:
		rcexpr(tr->tr1);
		C_com(wordsize);
		return;

	case QUEST:
		cbranch(tr->tr1, o1=isn++);
		rcexpr(tr->tr2->tr1);
		jump(o2 = isn++);
		fnlabel(o1);
		rcexpr(tr->tr2->tr2);
		fnlabel(o2);
		return;

	default:
		error("Can't print tree (op: %d)", tr->op);
	}

classerror:
	error("Storage class");
}

/* Prints the tree in RPN, for debugging */
/*
void
rcexpr(struct tnode *tr)
{
	struct hshtab *bs;

	if (tr == NULL)
		printf("(NULL) ");
	else if (tr->op == CON)
		printf("%d ", tr->value);
	else if (tr->op == STRING)
		printf("s(L%d) ", tr->value);
	else if (tr->op == NAME) {
		bs = (struct hshtab *)tr->tr1;
		if (bs->class == AUTO)
			printf("%s(%d) ", bs->name, bs->offset);
		else
			printf("%s ", bs->name);
	} else {
		rcexpr(tr->tr1);
		if (opdope[tr->op]&01)
			rcexpr(tr->tr2);
		printtoken(tr->op, stdout);
	}
}
*/
