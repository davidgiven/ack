/* cache.c */

#include <stdio.h>
#include </usr/include/assert.h>
#include <malloc.h>
#include <string.h>
#include <strings.h>
#include "mach.h"

#define free_reg_num(i) if (1) { assert(reg[i].inuse > 0); reg[i].inuse--; if (debug) fprintf(stderr,"free_reg(%s)\n", regnam[i]); } else

#define POP1 cache_need(1);
#define POP2 { --tos; assert(c_count); --c_count; }

int indent_count = 0;

#define enter(x) indent_count++;
#define indent() { int i = indent_count; while (i--) putc('\t', stderr); }
#define leave(x) indent_count--;

/* procedures:
		const13(int)		boolean proc
		init_cache()		mandatory
		free_reg(reg_t)
		forced_alloc_reg(reg_t)
		soft_alloc_reg(reg_t)
		change_reg(reg_t)	used when reg on stack has changed
		type_of_tos()		bit-field representation of ext/reg/cst
		inc_tos(int)		tos->cst += n
		push_const(int)
		push_reg(reg_t)
		push_ext(char *)
		flush_cache()		after branches and labels
		cache_read(int)		read-ahead. optimization only
		dump_cache(FILE *)	debug info: show current stack
		pop_nop()		remove element from cache

		reg_t alloc_reg()
		reg_t pop_reg()
		reg_t pop_reg_c13(char*)

		arith pop_const(char *)
		arith top_const()

		alloc_float, alloc_double, free_double,
		pop_float, pop_double, push_float, push_double

*/

typedef struct regdat_t {
		int	inuse;		/* free == 0, otherwise #owners */
} regdat_t;
/*
typedef struct cache_elt {
	reg_t	reg, reg2;
	char	*ext;
	arith	cst;
} cache_elt;
*/
char regnam[][8] = {
/*x*/	"%g0", "%g1", "%g2", "%g3", "%g4", "%g5", "%g6", "%g7",
/*x*/	"%i0", "%i1", "%i2", "%i3", "%i4", "%i5", "%i6", "%i7",
/*x*/	"%l0", "%l1", "%l2", "%l3", "%l4", "%l5", "%l6", "%l7",
/*x*/	"%o0", "%o1", "%o2", "%o3", "%o4", "%o5", "%o6", "%o7",
/*x*/	"%f0", "%f1", "%f2", "%f3", "%f4", "%f5", "%f6", "%f7",
/*x*/	"%f8", "%f9", "%f10", "%f11", "%f12", "%f13", "%f14", "%f15",
/*x*/	"%f16", "%f17", "%f18", "%f19", "%f20", "%f21", "%f22", "%f23",
/*x*/	"%f24", "%f25", "%f26", "%f27", "%f28", "%f29", "%f30", "%f31"
};

reg_t reg_g0, reg_g1, reg_g2, reg_g3, reg_g4, reg_g5, reg_g6, reg_g7;
reg_t reg_i0, reg_i1, reg_i2, reg_i3, reg_i4, reg_i5, reg_i6, reg_i7;
reg_t reg_l0, reg_l1, reg_l2, reg_l3, reg_l4, reg_l5, reg_l6, reg_l7;
reg_t reg_o0, reg_o1, reg_o2, reg_o3, reg_o4, reg_o5, reg_o6, reg_o7;
reg_t reg_f0;
reg_t reg_sp, reg_lb, reg_gap;
reg_t reg_tmp, reg_lin, reg_fil;

struct regdat_t reg[NR_REGS];

#define POP_SIZE 1	/* maybe >1  (read-ahead cache) or explicit?! */
#define CACHE_SIZE 32	/* ? */
#define LO_GLOB		0
#define HI_GLOB		7
#define LO_IN		8
#define HI_IN		15
#define LO_LOC		16
#define HI_LOC		23
#define LO_OUT		24
#define HI_OUT		31
#define LO_FLOAT	32
#define HI_FLOAT	63
const13(x) 
{
	return (x < 4096 && x >= -4096);
}

struct cache_elt cache[CACHE_SIZE], *tos = 0;
int c_count = 0;
const_str_t s;

static void panic(s)
char *s;
{
	printf("PANIC: %s\n", s);
	exit(1);
}

void init_cache()
{
	int i;

	for (i = 0; i < NR_REGS; i++)
		reg[i].inuse = 0;

	reg_g0 = regnam[000];
	reg_g1 = regnam[001];
	reg_g2 = regnam[002];
	reg_g3 = regnam[003];
	reg_g4 = regnam[004];
	reg_g5 = regnam[005];
	reg_g6 = regnam[006];
	reg_g7 = regnam[007];

	reg_i0 = regnam[010];
	reg_i1 = regnam[011];
	reg_i2 = regnam[012];
	reg_i3 = regnam[013];
	reg_i4 = regnam[014];
	reg_i5 = regnam[015];
	reg_i6 = regnam[016];
	reg_i7 = regnam[017];

	reg_l0 = regnam[020];
	reg_l1 = regnam[021];
	reg_l2 = regnam[022];
	reg_l3 = regnam[023];
	reg_l4 = regnam[024];
	reg_l5 = regnam[025];
	reg_l6 = regnam[026];
	reg_l7 = regnam[027];

	reg_o0 = regnam[030];
	reg_o1 = regnam[031];
	reg_o2 = regnam[032];
	reg_o3 = regnam[033];
	reg_o4 = regnam[034];
	reg_o5 = regnam[035];
	reg_o6 = regnam[036];
	reg_o7 = regnam[037];

	reg_f0 = regnam[040];

	reg_sp = reg_l0;
	reg_lb = reg_l1;
	reg_gap = reg_g4;
	reg_tmp = reg_o7;
	forced_alloc_reg(reg_g0);	/* can not be used as a reg */
	forced_alloc_reg(reg_o6);
	forced_alloc_reg(reg_o7);
	forced_alloc_reg(reg_i6);
	forced_alloc_reg(reg_i7);
}

static void flush_part_cache(n,r,f,d)
int n,r,f,d;
{
/* free at least n entries, r integer registers, f float regs and d double regs
 */
	int i, ready;
	int rn, j;
	const_str_t i_str, e_str, n_str;
	char *ext, R1, R2;
	reg_t rh, rt;

enter("flush_part_cache");
	for (i = 0; i < c_count; i++)
	{
		if (i >= n && !r && !f && !d)
			break;
		if (cache[i].reg != reg_g0)
		{
			rn= REG_NUM(cache[i].reg);
			free_reg_num(rn);
			if (!reg[rn].inuse)
				if (rn<LO_FLOAT)
				{
					if (r)
						r--;
				}
				else
				{
					if (d && (((rn & 1) &&
					!reg[rn-1].inuse) ||
					(!(rn & 1) && !reg[rn+1].inuse)))
						d--;
					if (f)
						f--;
				}
		}
		if (cache[i].reg2 != reg_g0)
		{
			rn= REG_NUM(cache[i].reg2);
			free_reg_num(rn);
			if (!reg[rn].inuse)
				if (rn<LO_FLOAT)
				{
					if (r)
						r--;
				}
				else
				{
					if (d && (((rn & 1) &&
					!reg[rn-1].inuse) ||
					(!(rn & 1) && !reg[rn+1].inuse)))
						d--;
					if (f)
						f--;
				}
		}
	}
	if (r || f || d)
		panic ("can't free enough registers");
	j = i;
	if (i)
	{
		sprintf (i_str, "%d", 4*i);
		"dec	$i_str, $reg_sp";
		while (i--)
		{
			sprintf(i_str, "%d", 4*(j-1-i));
			if (cache[i].ext)
			{
				ext= cache[i].ext;
				sprintf (e_str, "%d", cache[i].cst);
				"set	$ext+$e_str, $reg_tmp";
				"st	$reg_tmp, [$reg_sp+$i_str]";
				free(ext);
			}
			else
			{
				if (cache[i].reg2 != reg_g0)
				{
					rt = cache[i].reg;
					rh = cache[i].reg2;
					"add	$rt, $rh, $reg_tmp";
					cache[i].reg = reg_tmp;
					cache[i].reg2 = reg_g0;
				}
				if (!const13(cache[i].cst))
				{
					sprintf(n_str, "%d",
						cache[i].cst);
					"sethi	%hi($n_str), $reg_tmp";
					if (cache[i].reg != reg_g0)
					{
						rt = cache[i].reg;
						"add	$reg_tmp, $rt, $reg_tmp";
					}
					rh= reg_tmp;
					cache[i].cst &= 0x3ff;
				}
				else {
					rh= cache[i].reg;
				}
				if (cache[i].cst)
				{
					sprintf(n_str, "%d", cache[i].cst);
					"add	$rh, $n_str, $reg_tmp";
					rh= reg_tmp;
				}
				"st	$rh, [$reg_sp+$i_str]";
			}
		}
		for (i= j; i < c_count; i++)
			cache[i-j]= cache[i];
		c_count -= j;
		tos= &cache[c_count-1];
	}
leave("flush_part_cache");
}

reg_t alloc_reg()
{
	int i;
	reg_t res = NULL;

enter("alloc_reg");
	for (i = LO_GLOB+1 /* SPEED-HACK */; i <= HI_GLOB; i++) {
		if (reg[i].inuse)
			continue;
		reg[i].inuse = 1;	/* allocate */
		res = regnam[i];
		break;
	}
	if (!res)
		for (i = LO_IN; i <= HI_IN; i++) {
			if (reg[i].inuse)
				continue;
			reg[i].inuse = 1;	/* allocate */
			res = regnam[i];
			break;
		}
		if (!res)
			for (i = LO_LOC; i <= HI_LOC; i++) {
				if (reg[i].inuse)
					continue;
				reg[i].inuse = 1;	/* allocate */
				res = regnam[i];
				break;
			}
			if (!res)
				for (i = LO_OUT; i <= HI_OUT; i++) {
					if (reg[i].inuse)
						continue;
					reg[i].inuse = 1;	/* allocate */
					res = regnam[i];
					break;
				}
				if (!res) {
					flush_part_cache(c_count/2,1,0,0);
					res = alloc_reg();
				}
if (debug) { indent(); fprintf(stderr,"alloc_reg() = %s\n", res ? regnam[i] : "NULL"); }
leave("alloc_reg");
	return res;
}

reg_t alloc_float()
{
	int i;
	reg_t res = NULL;

enter("alloc_float");
	for (i = LO_FLOAT+15; i >= LO_FLOAT; i--) {
		if (reg[i].inuse)
			continue;
		reg[i].inuse = 1;	/* allocate */
		res = regnam[i];
		break;
	}
	if (!res) {
		flush_part_cache(c_count/2,0,1,0);
		res = alloc_float();
	}
if (debug) { indent(); fprintf(stderr,"alloc_float() = %s\n", res ? regnam[i] : "NULL"); }
leave("alloc_float");
	return res;
}

reg_t alloc_double(sub_reg)
reg_t *sub_reg;
{
	int i;
	reg_t res = NULL;

enter("alloc_double");
	for (i = LO_FLOAT+14; i >= LO_FLOAT; i -= 2) {
		if (reg[i].inuse || reg[i+1].inuse)
			continue;
		reg[i].inuse = 1;	/* allocate */
		reg[i+1].inuse = 1;	/* allocate */
if (debug) { indent(); fprintf(stderr,"alloc_double() = %s\n", regnam[i]); }
		if (sub_reg)
			*sub_reg= regnam[i+1];
		res = regnam[i];
		break;
	}
	if (!res) {
		flush_part_cache(c_count/2,0,0,1);
		res = alloc_double(sub_reg);
	}
if (debug) { indent(); fprintf(stderr,"alloc_double() = %s\n", res ? regnam[i] : "NULL"); }
leave("alloc_double");
	return res;
}

reg_t alloc_reg_var()		/* ins and locals only */
{
	int i;
	reg_t res = NULL;

enter("alloc_reg_var");
	for (i = LO_LOC +2 /* SPEED-HACK */; i <= HI_LOC; i++) {
		if (reg[i].inuse)
			continue;
		reg[i].inuse = 1;	/* allocate */
		res = regnam[i];
		break;
	}
	if (!res)
		for (i = LO_IN; i <= HI_IN; i++) {
			if (reg[i].inuse)
				continue;
			reg[i].inuse = 1;	/* allocate */
			res = regnam[i];
			break;
		}
if (debug) { indent(); fprintf(stderr,"alloc_reg_var() = %s\n", res ? regnam[i] : "NULL"); }
leave("alloc_reg_var");
	return res;
}

reg_t alloc_float_var()
{
	int i;
	reg_t res = NULL;

enter("alloc_float_var");
	for (i= LO_FLOAT+16; i<= HI_FLOAT; i++)
	{
		if (reg[i].inuse)
			continue;
		reg[i].inuse = 1;	/* allocate */
		res = regnam[i];
		break;
	}
if (debug) { indent(); fprintf(stderr,"alloc_reg_var() = %s\n", res ? regnam[i] : "NULL"); }
leave("alloc_float_var");
	return res;
}

reg_t alloc_double_var(sub_reg)
reg_t *sub_reg;
{
	int i;
	reg_t res = NULL;

enter("alloc_double_var");
	for (i = LO_FLOAT+16; i < HI_FLOAT; i += 2) {
		if (reg[i].inuse || reg[i+1].inuse)
			continue;
		reg[i].inuse = 1;	/* allocate */
		reg[i+1].inuse = 1;	/* allocate */
if (debug) { indent(); fprintf(stderr,"alloc_double_var() = %s\n", regnam[i]); }
		if (sub_reg)
			*sub_reg= regnam[i+1];
		res = regnam[i];
		break;
	}
if (debug) { indent(); fprintf(stderr,"alloc_double_var() = %s\n", res? regnam[i] : "NULL"); }
leave("alloc_double_var");
	return res;
}

void free_reg(i)
reg_t i;
{
	int r;

enter("free_reg");
	if (i != 0 && i != reg_g0) {
		r = REG_NUM(i);
		assert(0 <= r && r <= NR_REGS);
		assert(reg[r].inuse > 0);	/* "freeing unused register" */
		reg[r].inuse--;
	}
if (debug) { indent(); fprintf(stderr,"free_reg(%s)\n", i); }
leave("free_reg");
}

void free_double_reg(i)
reg_t i;
{
	int rn;

enter("free_double_reg");
	rn= REG_NUM(i);
	assert (!(rn & 1));
	free_reg(i);
	free_reg(regnam[rn+1]);
leave("free_double_reg");
}


void force_alloc_output()	/* TMP HACK */
{
	int i;

enter("force_alloc_output");
if (debug) { indent(); fprintf(stderr,"force_alloc_output\n"); }
	for (i = REG_NUM(reg_o0); i <= REG_NUM(reg_o5); i++)
		forced_alloc_reg(regnam[i]);
leave("force_alloc_output");
}

void free_output()
{
	int i;

enter("free_output");
leave("free_output");
if (debug) { indent(); fprintf(stderr,"free_output\n"); }
	for (i = REG_NUM(reg_o0); i <= REG_NUM(reg_o5); i++) {
		assert(reg[i].inuse > 0);
		reg[i].inuse--;
	}
}

void soft_alloc_reg(i)
reg_t i;
{
enter("soft_alloc_reg");
if (debug) { indent(); fprintf(stderr,"soft_alloc_reg(%s)\n", i); }
	/* assert(reg[REG_NUM(i)].inuse); */
	reg[REG_NUM(i)].inuse++;
leave("soft_alloc_reg");
}

void forced_alloc_reg(i)
reg_t i;
{
	int r;

enter("forced_alloc_reg");
if (debug) { indent(); fprintf(stderr,"forced_alloc_reg(%s)\n", i); }

	r = REG_NUM(i);

	if (reg[r].inuse) {
		reg_t S1;
		char *V2;

		S1 = alloc_reg();
if (debug) { indent(); fprintf(stderr,"---> inuse: moving to %s\n", S1); }
		"mov	$i, $S1";
		subst_reg(i, S1);
		free_reg(S1);
		if (reg[r].inuse)
			panic("forced_alloc_reg: external owners left!");
	}
	reg[r].inuse = 1;
leave("forced_alloc_reg");
}

void change_reg(r)	/* to be called when register r changes */
reg_t r;
{
	int i;

enter("change_reg");
if (debug) { indent(); fprintf(stderr, "change_reg(%s)\n", r); }
	if (r != reg_g0)
		for (i = 0; i < c_count; i++)
			if (cache[i].reg == r || cache[i].reg2 == r) {
				reg_t S1;

				if (r >= reg_f0) {
					S1 = alloc_float();
					"fmovs	$r, $S1";
				} else {
					S1 = alloc_reg();
					"mov	$r, $S1";
				}
				subst_reg(r, S1);
				free_reg(S1);
				break;
			}
leave("change_reg");
}

static void subst_reg(old, new)
reg_t old, new;
{
	int i;

enter("subst_reg");
if (debug) { indent(); fprintf(stderr,"subst_reg(%s, %s)\n", old, new); }
	for (i = 0; i < c_count; i++) {
		if (cache[i].reg == old) {
			cache[i].reg = new;
			free_reg(old);
			soft_alloc_reg(new);
		}
		if (cache[i].reg2 == old) {
			cache[i].reg2 = new;
			free_reg(old);
			soft_alloc_reg(new);
		}
	}
leave("subst_reg");
}

int type_of_tos()
{
	int r = 0;

	cache_need(1);

	if (tos->ext)
		r |= T_ext;
	if (tos->reg != reg_g0)
		if (tos->reg >= reg_f0)
			r |= T_float;
		else
			r |= T_reg;
	if (tos->reg2 != reg_g0)
		if (tos->reg2 >= reg_f0)
			r |= T_float2;
		else
			r |= T_reg2;
	if (tos->cst)
		r |= T_cst;
	return r;
}

arith top_const()
{
enter("top_const");
	assert(type_of_tos() == T_cst);
if (debug) { indent(); fprintf(stderr,"top_const()=%d\n", tos->cst); }
leave("top_const");
	return tos->cst;
}

reg_t pop_reg_reg(r)
reg_t *r;
{
	reg_t s;

enter("pop_reg_reg");
	POP1;
	if (!(type_of_tos() & T_reg2))
		push_reg(pop_reg());
	assert(r);
	*r = tos->reg2;
	s = tos->reg;
	POP2;
if (debug) { indent(); fprintf(stderr,"pop_reg_reg()=%s\n", s); fprint(codefile,"\t\t! "); dump_cache(codefile); }
leave("pop_reg_reg");
	return s;
}

reg_t pop_reg_c13(n)		/* returns reg_t + optional n (as string) */
char *n;
{
	reg_t S1, S2, S3;
	register char *V1;
	const_str_t V2;
enter("pop_reg_c13");
if (debug) { indent(); fprintf(stderr,"pop_reg_c13()=...\n"); }
	*n = '0';
	*(n+1) = 0;
	cache_need(1);

	if (tos->reg >= reg_f0) {	/* convert float to int */
		S1= pop_float();
		S2= alloc_reg();
		"st	$S1, [%fp+64]";
		"ld	[%fp+64], $S2";
		free_reg(S1);
		S1 = S2;
	}
	else if (tos->reg2 != reg_g0) {	/* add integers */
		S1 = pop_reg();
	}
	else if (tos->ext) {
		assert(tos->reg == reg_g0);
		S1 = alloc_reg();
		V1 = tos->ext;
		sprintf(V2, "%d", tos->cst);
		"sethi	%hi($V1+$V2), $S1";
		sprintf(n, "%%lo(%s+%d)", tos->ext, tos->cst);
		free(V1);
		POP2;
	} else {
		S1 = tos->reg;
		if (!(const13(tos->cst))) {
			S3 = alloc_reg();
			sprintf(V2, "%d", tos->cst);
			"sethi	%hi($V2), $S3";
			if (tos->reg != reg_g0) {
				S2 = alloc_reg();
				"add	$S3, $S1, $S2";
				free_reg(S1);
				free_reg(S3);
				S1 = S2;
			}
			tos->cst &= 0x3FF;
		}
		sprintf(n, "%d", tos->cst);
		POP2;
	}
if (debug) { indent(); fprint(codefile, "\t\t! %s+%s cache:", S1, n); dump_cache(codefile);}
leave("pop_reg_c13");
	return S1;
}

reg_t pop_float()
{
	reg_t S1, R1, R2;
	char *V1, *V2;

enter("pop_float");
	cache_need(1);
	S1 = tos->reg;
	if (!(type_of_tos() & T_float)) {
		S1 = pop_reg();
		R2 = alloc_float();
		"st	$S1, [%fp+64]";
		"ld	[%fp+64], $R2";
		free_reg(S1);
		S1 = R2;
	}
	else if (tos->reg2 >= reg_f0) {
		R1 = tos->reg2;
		R2 = alloc_float();
		"fadds	$S1, $R1, $R2";
		free_reg(S1);	
		free_reg(R1);	
		S1 = R2;
		POP2;
	} else
		POP2;
if (debug) { indent(); fprint(codefile, "\t\t! %s cache:", S1); dump_cache(codefile); }
leave("pop_float");
	return S1;
}

void inc_tos_reg(r)
reg_t r;
{
enter("inc_tos_reg");
if (debug) { indent(); fprintf(stderr, "inc_tos_reg(%s)\n", r); }
	if (type_of_tos() != T_reg)
		push_reg(pop_reg());
	tos->reg2 = r;
if (debug) { indent(); fprint(codefile, "\t\t! "); dump_cache(codefile); }
leave("inc_tos_reg");
}

void inc_tos(n)
arith n;
{
	reg_t S1;
	char *V1, *V2;

enter("inc_tos");
if (debug) { indent(); fprintf(stderr,"inc_tos(%d)\n", n); }
	cache_need(1);
	if (tos->reg >= reg_f0)
	{
		S1= pop_reg();
		push_reg(S1);
	}
	else if (tos->reg2 != reg_g0) {
		S1= pop_reg();
		push_reg(S1);
	}
	tos->cst += n;
if (debug) { indent(); fprint(codefile, "\t\t! "); dump_cache(codefile); }
leave("inc_tos");
}

#define INC_TOS	if (c_count >= CACHE_SIZE) flush_part_cache(c_count/2,0,0,0); \
			tos = &cache[c_count++];

void push_const(n)
arith n;
{
enter("push_const");
	INC_TOS;
	tos->reg = reg_g0;
	tos->reg2 = reg_g0;
	tos->ext = 0;
	tos->cst = n;
if (debug) { indent(); fprint(codefile, "\t\t! "); dump_cache(codefile); }
leave("push_const");
}

void push_reg(i)
reg_t i;
{
enter("push_reg");
	assert(0 <= REG_NUM(i) && REG_NUM(i) < NR_REGS);
	INC_TOS;
	tos->reg = i;
	tos->reg2 = reg_g0;
	tos->ext = 0;
	tos->cst = 0;
if (debug) { indent(); fprint(codefile, "\t\t! "); dump_cache(codefile); }
leave("push_reg");
}


void push_double_reg(i)
reg_t i;
{
	int rn;

enter("push_double_reg");
	rn= REG_NUM(i);
	assert (!(rn & 1));
	INC_TOS;
	tos->ext = 0;
	tos->cst = 0;
	tos->reg = regnam[rn+1];
	tos->reg2 = reg_g0;
	INC_TOS;
	tos->ext = 0;
	tos->cst = 0;
	tos->reg = i;
	tos->reg2 = reg_g0;
if (debug) { indent(); fprint(codefile, "\t\t! "); dump_cache(codefile); }
leave("push_double_reg");
}

void push_ext(s)
char *s;
{
	char *p;

enter("push_ext");
	p = malloc(strlen(s)+1);
	assert(p);

	INC_TOS;
	tos->reg = reg_g0;
	tos->reg2 = reg_g0;
	tos->ext = strcpy(p, s);
	tos->cst = 0;
if (debug) { indent(); fprint(codefile, "\t\t! "); dump_cache(codefile); }
leave("push_ext");
}

arith pop_const(n)
char *n;
{
	arith x;

enter("pop_const");
	POP1;
	x = top_const();
	POP2;
	if (n)
		sprintf(n, "%d", x);
if (debug) { indent(); fprint(codefile, "\t\t! %d cache:", x); dump_cache(codefile); }
leave("pop_const");
	return x;
}

void pop_reg_as(r)
reg_t r;
{
	reg_t S1, S2;
	register char *V1, *V3;
	const_str_t V2, c_str;
	char *tos_ext;
	reg_t tos_reg, tos_reg2;
	int tos_cst;

enter("pop_reg_as");
if (debug) { indent(); fprintf(stderr,"pop_reg_as(%s)=...\n", r); }
	if (c_count == 0) {	/* special case */
		"ld	[%l0], $r";
		"inc	4, %l0";
	} else if (r >= reg_f0) {
		if (tos->reg < reg_f0)
		{
			S1= pop_reg();
			change_reg(r);
			"st	$S1, [%fp+64]";
			"ld	[%fp+64], $r";
			free_reg(S1);
		}
		else
		{
			if (tos->reg2 == reg_g0) {
				S1= pop_float();
				change_reg(r);
				if (S1 != r)
					"fmovs	$S1, $r";
				free_reg(S1);
			} else {
				V1 = tos->reg;
				V3 = tos->reg2;
				POP2;
				change_reg(r);
				"fadds	$V1, $V3, $r";
				free_reg(V1);
				free_reg(V3);
			}
		}
	} else if (tos->reg >= reg_f0) {
		S1= pop_float();
		change_reg(r);
		"st	$S1, [%fp+64]";
		"ld	[%fp+64], $r";
		free_reg(S1);
	} else if (tos->ext) {
		assert(tos->reg == reg_g0);
		V1 = tos->ext;
		sprintf(V2, "%d", tos->cst);
		"set	$V1+$V2, $r";
		free(V1);
		POP2;
	} else {
		POP1;
		tos_reg= tos->reg;
		tos_reg2= tos->reg2;
		tos_cst= tos->cst;
		POP2;
		change_reg(r);
		if (!const13(tos_cst))
		{
			assert (tos_reg2 == reg_g0);
			if (tos_reg != reg_g0 || (tos_cst & 0x3ff))
				tos_reg2= alloc_reg();
			else
			{
				soft_alloc_reg(r);
				tos_reg2= r;
			}
			sprintf(c_str, "%d", tos_cst);
			"sethi	%hi($c_str), $tos_reg2";
			tos_cst &= 0x3ff;
			if (tos_reg == reg_g0)
			{
				tos_reg= tos_reg2;
				tos_reg2= reg_g0;
			}
		}
		if (tos_reg2 != reg_g0)
		{
			assert (tos_reg != reg_g0);
			if (tos_cst)
				S1= alloc_reg();
			else
			{
				soft_alloc_reg(r);
				S1= r;
			}
			"add	$tos_reg, $tos_reg2, $S1";
			free_reg(tos_reg);
			free_reg(tos_reg2);
			tos_reg= S1;
			tos_reg2= reg_g0;
		}
		if (tos_cst)
		{
			sprintf(c_str, "%d", tos_cst);
			soft_alloc_reg(r);
			"add	$tos_reg, $c_str, $r";
			free_reg(tos_reg);
			tos_reg= r;
		}
		if (tos_reg != r)
			"mov	$tos_reg, $r";
		free_reg(tos_reg);
	}
leave("pop_reg_as");
}

void pop_double_reg_as(rdl)
reg_t rdl;
{
	reg_t rl, rh;
	reg_t rdh;
	reg_t t;
	int rn;
	
enter("pop_double_reg_as");
	rn= REG_NUM(rdl);
	assert (!(rn & 1));
	rdh= regnam[rn+1];
	if (rdl>= reg_f0)
	{
		if (c_count == 0) {
			"ld	[%l0], $rdl";
			"ld	[%l0+4], $rdh";
			"inc	8, %l0";
		} else {
			if (type_of_tos() & T_float)
				rl= pop_float();
			else
				rl= pop_reg();
			if (type_of_tos() & T_float)
				rh= pop_float();
			else
				rh= pop_reg();
			change_reg(rdl);
			change_reg(rdh);
			if (rl < reg_f0 && rh < reg_f0)
			{
				"st	$rl, [%fp+64]";
				"st	$rh, [%fp+68]";
				"ldd	[%fp+64], $rdl";
				free_reg(rl);
				free_reg(rh);
			}
			else if (rl < reg_f0)
			{
				if (rh != rdh)
					"fmovs	$rh, $rdh";
				"st	$rl, [%fp+64]";
				"ld	[%fp+64], $rdl";
				free_reg(rl);
				free_reg(rh);
			}
			else if (rh < reg_f0)
			{
				if (rl != rdl)
					"fmovs	$rl, $rdl";
				"st	$rh, [%fp+64]";
				"ld	[%fp+64], $rdh";
				free_reg(rl);
				free_reg(rh);
			} else {
				if (rdl == rl)
					free_reg(rl);
				if (rdh == rh)
					free_reg(rh);
				if (rdl == rh && rdh == rl)
				{
					t= alloc_float();
					"fmovs	$rl, $t";
					free_reg(rl);
					rl= t;
				}
				if (rdl != rl && rdl != rh)
				{
					"fmovs	$rl, $rdl";
					free_reg(rl);
					rl= rdl;
				}
				if (rdh != rh && rdh != rl)
				{
					"fmovs	$rh, $rdh";
					free_reg(rh);
					rh= rdh;
				}
				if (rdl != rl && rdl != rh)
				{
					"fmovs	$rl, $rdl";
					free_reg(rl);
					rl= rdl;
				}
				assert (rdl == rl && rdh == rh);
			}
		}
	}
	else
	{
		pop_reg_as (rdl);
		pop_reg_as (rdh);
	}
leave("pop_double_reg_as");
}

			
		
reg_t pop_reg()
{
	reg_t S1;

enter("pop_reg");
	POP1;
	if (type_of_tos() == T_reg) {
		S1 = tos->reg;
		POP2;
	} else {
		S1 = alloc_reg();
		pop_reg_as(S1);
	}
if (debug) { indent(); fprint(codefile, "\t\t! %s cache:", S1); dump_cache(codefile); }
leave("pop_reg");
	return S1;
}


reg_t pop_double(sub_reg)	/* pop_double_float actually */
reg_t *sub_reg;
{
	reg_t R1, R2, R3, R4;
	char *V1, *V2;

enter("pop_double");
	if (c_count == 0) {
		R1 = alloc_double(&R2);
		"ld	[%l0], $R1";
		"ld	[%l0+4], $R2";
		"inc	8, %l0";
		if (sub_reg)
			*sub_reg = R2;
	} else {
		cache_need(2);
		if (tos->reg >= reg_f0 && tos[-1].reg >= reg_f0 &&
			REG_NUM(tos->reg) == REG_NUM(tos[-1].reg)-1 &&
			tos->reg2 == reg_g0 && tos[-1].reg2 == reg_g0)
		{
			R1= tos->reg;
			if (sub_reg)
				*sub_reg= tos[-1].reg;
			POP2;
			POP2;
		} else {
			R1= alloc_double(&R2);
			if (sub_reg)
				*sub_reg= R2;
			if (tos->reg >= reg_f0 || tos[-1].reg >= reg_f0)
			{
				pop_reg_as(R1);
				pop_reg_as(R2);
			} else {
				/* two normal registers */
				R3= pop_reg();
				R4= pop_reg();
				"st	$R3, [%fp+64]";
				"st	$R4, [%fp+68]";
				"ldd	[%fp+64], $R1";
				free_reg(R3);
				free_reg(R4);
			}
		}
	}
leave("pop_double");
	return R1;
}

void pop_nop(i)
int i;
{
	const_str_t V1;
	int j = i;

enter("pop_nop");
	while (c_count && i) {
		i--;
		POP1;
		free_reg(tos->reg);
		free_reg(tos->reg2);
		if (tos->ext)
			free(tos->ext);
		POP2;
	}
	if (i) {
		sprintf(V1, "%d", 4*i);
		if (const13(4*i)) {
			"inc	$V1, %l0";
		} else {
			"set	$V1, $reg_tmp";
			"inc	%l0, $reg_tmp, %l0";
		}
	}
if (debug) { indent(); fprint(codefile, "\t\t! %dw cache:",j); dump_cache(codefile); }
leave("pop_nop");
}

check_cache()
{
	int i;
if (debug)
	for (i = 0; i < NR_REGS; i++)
		fprintf(stderr, "%c", reg[i].inuse ? (reg[i].inuse)+48 : '.');
	fprintf(stderr, "\n");
}

void flush_cache()		/* flush the cache onto the stack */
{
enter("flush_cache");

if (debug) { indent(); fprintf(stderr,"flush_cache\n"); }
	if (c_count)
		flush_part_cache(c_count, 0, 0, 0);

	/* TEST */
if (debug)
	check_cache();
leave("flush_cache");
}

void cache_need(n)
int n;
{
enter("cache_need");
	if (c_count < n)
		(void) cache_read(n, 0);
	assert(c_count >= n);
leave("cache_need");
}

static int cache_read(n, i)
int n;
int i;
{
	const_str_t V1;
	reg_t S1;
	int j;
	int old_c_count;

enter("cache_read");
if (debug) { indent(); fprintf(stderr,"cache_read(%d, %d)\n", n,i); }
	if (i+c_count<n)
	{
		S1= alloc_reg();
		old_c_count = cache_read(n, i+1);

		sprintf(V1, "%d", (old_c_count-1-i) * 4);
		"ld	[%l0+$V1], $S1";
		cache[i].reg= S1;
		cache[i].reg2= reg_g0;
		cache[i].ext= 0;
		cache[i].cst= 0;
		if (!i)
		{
			sprintf(V1, "%d", (old_c_count)*4);
			"add	$reg_sp, $V1, $reg_sp";
		}
	}
	else
	{
		assert (i);
		for (j= c_count-1; j>=0; j--)
			cache[j+i]= cache[j];
		c_count += i;
		tos= &cache[c_count-1];
		old_c_count = i;
	}
leave("cache_read");
	return old_c_count;
}

static void dump_cache(stream)		/* to codefile! */
File *stream;
{
	int i;

	assert (c_count >= 0);
	for (i = c_count -1; i >= 0; i--) {
		if (cache[i].ext)
			fprint(stream, "%s", cache[i].ext);
		if (cache[i].reg != reg_g0) {
			if (cache[i].ext)
				fprint(stream, "+");
			fprint(stream, "%s", cache[i].reg);
			if (cache[i].reg2 != reg_g0) {
				fprint(stream, "+");
				fprint(stream, "%s", cache[i].reg2);
			}
		}
		if (cache[i].cst || (!cache[i].ext && cache[i].reg == reg_g0)) {
			if (cache[i].ext || cache[i].reg != reg_g0)
				fprint(stream, "+");
			fprint(stream, "%d", cache[i].cst);
		}
		fprint(stream, " ");
	}
	fprint(stream, "\n");
if (debug) check_cache();
}

void dup_tos(n)
int n;
{
	reg_t a;
	int i;
	const_str_t i_str;

enter("dup_tos");
	for (i = 0;i < n; i++) {
		INC_TOS;
		tos->reg = reg_g0;
		tos->reg2 = reg_g0;
		tos->cst = 0;
		tos->ext = 0;
		if (c_count > n) {
			char *ext;

			*tos = tos[-n];
			if (tos->ext)
			{
				ext= malloc(strlen(tos->ext)+1);
				strcpy(ext, tos->ext);
				tos->ext= ext;
			}
			soft_alloc_reg(tos->reg);
			soft_alloc_reg(tos->reg2);
		} else {
			a= alloc_reg();
			sprintf(i_str, "%d", (n-c_count)*4);
			"ld	[$reg_sp+$i_str], $a";
			tos->reg = a;
		}
	}
if (debug) { indent(); fprint(codefile, "\t\t! "); dump_cache(codefile); }
leave("dup_tos");
}
