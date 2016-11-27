#include "b.h"

/*
 * Code generation (x86 assembly)
 */

void
push(void)
{
	printf("\tpush\t%%eax\n");
}

void
pop(char *s)
{
	printf("\tpop\t%%%s\n", s);
}

void
binary(struct tnode *tr)
{
	rcexpr(tr->tr1);
	push();
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
		push();
		stk = pushargs(tr->tr1);
		return stk+NCPW;
	}
	rcexpr(tr);
	push();
	return NCPW;
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
			printf("\tmov\t%%eax,%%ebx\n");
			sprintf(memloc,"(,%%ebx,4)");
		} else {	/* NAME, checked in "build" */
			bs = (struct hshtab *) tr->tr1->tr1;
			if (bs->class == EXTERN)
				sprintf(memloc,"_%s", bs->name);
			else if (bs->class == AUTO)
				sprintf(memloc,"%d(%%ebp)", bs->offset);
			else
				goto classerror;
		}
		if (tr->op == DECBEF || tr->op == INCBEF) {
			printf("\t%s\t%s\n", tr->op == DECBEF ? "decl" : "incl",
			       memloc);
			printf("\tmov\t%s,%%eax\n", memloc);
		} else {
			printf("\tmov\t%s,%%eax\n", memloc);
			printf("\t%s\t%s\n", tr->op == DECAFT ? "decl" : "incl",
			       memloc);
		}
		return;

	case ASSIGN:
		rcexpr(tr->tr2);
		if (tr->tr1->op == STAR) {
			push();
			rcexpr(tr->tr1->tr1);
			pop("ebx");
			printf("\tmov\t%%ebx,(,%%eax,4)\n");
		} else {	/* NAME */
			bs = (struct hshtab *) tr->tr1->tr1;
			if (bs->class == EXTERN)
				printf("\tmov\t%%eax,_%s\n", bs->name);
			else if (bs->class == AUTO)
				printf("\tmov\t%%eax,%d(%%ebp)\n", bs->offset);
			else
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
		printf("\tmov\t$%d,%%eax\n", tr->value);
		return;

	case STRING:
		printf("\tmov\t$L%d,%%eax\n", tr->value);
		printf("\tshr\t$2,%%eax\n");
		return;

	case NAME:	/* only rvalue */
		bs = (struct hshtab *) tr->tr1;
		if (bs->class == EXTERN)
			printf("\tmov\t_%s,%%eax\n", bs->name);
		else if (bs->class == AUTO)
			printf("\tmov\t%d(%%ebp),%%eax\n", bs->offset);
		else
			goto classerror;
		return;

	case CALL:
		stk = pushargs(tr->tr2);
		rcexpr(tr->tr1);
		printf("\tshl\t$2,%%eax\n");
		printf("\tcall\t*%%eax\n");
		if (stk)
			printf("\tadd\t$%d,%%esp\n",stk);
		return;

	case AMPER:
		bs = (struct hshtab *) tr->tr1->tr1;
		if (bs->class == EXTERN) {
			printf("\tmov\t$_%s,%%eax\n", bs->name);
			printf("\tshr\t$2,%%eax\n");
		} else if (bs->class == AUTO) {
			printf("\tlea\t%d(%%ebp),%%eax\n", bs->offset);
			printf("\tshr\t$2,%%eax\n");
		} else
			goto classerror;
		return;

	case STAR:	/* only rvalue */
		rcexpr(tr->tr1);
		printf("\tmov\t(,%%eax,4),%%eax\n");
		return;

	case PLUS:
		binary(tr);
		pop("ebx");
		printf("\tadd\t%%ebx,%%eax\n");
		return;

	case MINUS:
		binary(tr);
		printf("\tmov\t%%eax,%%ebx\n");
		pop("eax");
		printf("\tsub\t%%ebx,%%eax\n");
		return;

	case TIMES:
		binary(tr);
		pop("ebx");
		printf("\tmul\t%%ebx\n");
		return;

	case DIVIDE:
		binary(tr);
		printf("\tmov\t%%eax,%%ebx\n");
		pop("eax");
		printf("\txor\t%%edx,%%edx\n");
		printf("\tdiv\t%%ebx\n");
		return;

	case MOD:
		binary(tr);
		printf("\tmov\t%%eax,%%ebx\n");
		pop("eax");
		printf("\txor\t%%edx,%%edx\n");
		printf("\tdiv\t%%ebx\n");
		printf("\tmov\t%%edx,%%eax\n");
		return;

	case AND:
		binary(tr);
		pop("ebx");
		printf("\tand\t%%ebx,%%eax\n");
		return;

	case OR:
		binary(tr);
		pop("ebx");
		printf("\tor\t%%ebx,%%eax\n");
		return;

	case LSHIFT:
		binary(tr);
		printf("\tmov\t%%eax,%%ecx\n");
		pop("eax");
		printf("\tshl\t%%cl,%%eax\n");
		return;

	case RSHIFT:
		binary(tr);
		printf("\tmov\t%%eax,%%ecx\n");
		pop("eax");
		printf("\tshr\t%%cl,%%eax\n");
		return;

	case EQUAL:
	case NEQUAL:
	case LESS:
	case LESSEQ:
	case GREAT:
	case GREATEQ:
		binary(tr);
		pop("ebx");
		printf("\tcmp\t%%eax,%%ebx\n");
		switch (tr->op) {
		case EQUAL:
			printf("\tsete\t%%al\n");
			break;
		case NEQUAL:
			printf("\tsetne\t%%al\n");
			break;
		case LESS:
			printf("\tsetl\t%%al\n");
			break;
		case LESSEQ:
			printf("\tsetle\t%%al\n");
			break;
		case GREAT:
			printf("\tsetg\t%%al\n");
			break;
		case GREATEQ:
			printf("\tsetge\t%%al\n");
			break;
		}
		printf("\tmovzb\t%%al,%%eax\n");
		return;

	case EXCLA:
		rcexpr(tr->tr1);
		printf("\ttest\t%%eax,%%eax\n");
		printf("\tsete\t%%al\n");
		printf("\tmovzb\t%%al,%%eax\n");
		return;

	case NEG:
		rcexpr(tr->tr1);
		printf("\tneg\t%%eax\n");
		return;

	case QUEST:
		cbranch(tr->tr1, o1=isn++, 0);
		rcexpr(tr->tr2->tr1);
		jump(o2 = isn++);
		label(o1);
		rcexpr(tr->tr2->tr2);
		label(o2);
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
