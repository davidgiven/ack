/* lfr ret should little endian */

#define const13(x) ((x) > -4096 && (x) < 4096)
#define NULL 0
#include "mach_em.h"

define(`RETH_LD',`reg_o1')
define(`RETL_LD',`reg_o0')
define(`RETH_ST',`reg_i1')
define(`RETL_ST',`reg_i0')
define(`LIN_NO',`%g6')
define(`FIL_NAM',`%g7')
define(O0, reg_o0)
define(O1, reg_o1)
define(O2, reg_o2)
define(O3, reg_o3)

define(`BP_OFFSET',`'WINDOWSIZE)
define(`'`EM_BSIZE',EM_BSIZE)
define(STACK_CLICK,4)

#if RESOLV_debug
define(Comment0)
define(Comment)
define(Comment2)
#else
define(Comment0,; `'`)' ; `'`/* */'"	! $1"		; code_combiner`'`(' )
define(Comment, ; `'`)' ; `'`/* */'"	! $1 $2"	; code_combiner`'`(' )
define(Comment2,; `'`)' ; `'`/* */'"	! $1 $2 $3"	; code_combiner`'`(' )
#endif

define(MAX_INT, 0x7fffffff)
define(E_EM_CUF, 100)
define(E_EM_CFF, 101)
define(E_EM_CFI, 102)
define(E_EM_CFU, 103)
#define MAX_UNROLL	16
#undef FAST_LIN_LNI_FIL


define( narg4,
C_$1_narg	==>
`	{
		reg_t a;
		int n;
		
		Comment0( $1_narg );
		if (type_of_tos() == T_cst)
		{
			n= pop_const(NULL);
			C_$1 (n);
		}
		else
		{
			a= pop_reg();
			force_alloc_output();
			"cmp	$a, 4";
			"be	1f";
			"set	EILLINS, $reg_o0";
			"call	trp";
			"nop";
		"1:";
			free_reg(a);
			free_output();
		C_$1 (4);
		}
	}.
'
)


/******************************************************************************/
/*                                                                            */
/*			Group 1 : Load instructions			      */
/*							 		      */
/******************************************************************************/

/*	%fp : frame pointer
 *	%sp : stack pointer
 *	RETH_XX: High part of return value
 *	RETL_XX: Low part of return value
 *	LIN_NO : lin_no
 *	FIL_NAM: Fil_nam
 */

C_loc		==>
			Comment( loc , $1 );
			push_const($1).


C_lol		==>
		Comment( lol , $1 );
		{
			reg_t S1;

			if (S1 = find_local($1, NULL)) {
				soft_alloc_reg(S1);
				push_reg(S1);
			} else {
				soft_alloc_reg(reg_lb);
				push_reg(reg_lb);
				inc_tos($1);
				push_const(4);
				C_los(EM_WSIZE);
			}
		}.


C_loe..		==>
		Comment2( loe.. , $1, $2 );
		{
#ifdef FAST_LIN_LNI_FIL
			if ((int*)($1) == (int*)"hol0")
				if ($2 == 0)
					push_reg(reg_lin);
				else if ($2 == 4)
					push_reg(reg_fil);
				else
					arg_error("loe.. hol0+", $2);
			else {
#endif
				push_ext($1);
				inc_tos($2);
				push_const(4);
				C_los(EM_WSIZE);
#ifdef FAST_LIN_LNI_FIL
			}
#endif
		}
		.

C_lil		==>
		Comment( lil , $1 );
		{
			reg_t S1;
			reg_t S2;

			if (S1 = find_local($1, NULL)) {
				S2 = alloc_reg();
				"ld	[$S1], $S2";
				push_reg(S2);
			} else {
				soft_alloc_reg(reg_lb);
				push_reg(reg_lb);
				inc_tos($1);
				push_const(4);
				C_los(EM_WSIZE);
				push_const(4);
				C_los(EM_WSIZE);
			}
		}.

C_lof		==>
			Comment( lof , $1 );
			inc_tos($1);
			push_const(4);
			C_los(EM_WSIZE).

C_lal		==>
			Comment( lal , $1 );
			soft_alloc_reg(reg_lb);
			push_reg(reg_lb);
			inc_tos($1).

C_lae..		==>
			Comment2( lae.. , $1, $2 );
			push_ext($1);
			inc_tos($2).

C_lxl
	$1 == 0	==>
			Comment( lxl , $1 );
			soft_alloc_reg(reg_lb);
			push_reg(reg_lb).
	$1 == 1	==>
			Comment( lxl , $1 );
			soft_alloc_reg(reg_lb);
			push_reg(reg_lb);
			inc_tos(EM_BSIZE);
			push_const(4);
			C_los(EM_WSIZE).
	default ==>
			Comment( lxl , $1 );
			{
				reg_t a;
				reg_t b;
				reg_t c;
				const_str_t n_str;

				a = alloc_reg();
				b = alloc_reg();
				c = alloc_reg();
				sprint(n_str, "%d", $1);
				"set	$n_str, $a";
				"mov 	$reg_lb, $b";
			"1:	ld	[$b + EM_BSIZE], $c";
				"deccc	$a";
				"bnz	1b";
				"mov	$c, $b";
				push_reg(b);
				free_reg(a);
				free_reg(c);
			}.

C_lxa	==>
	C_lxl($1);
	inc_tos(EM_BSIZE).

C_loi
	( $1 == 1 ) ||
	( $1 == 2 ) ||
	( $1 % 4 == 0 )	==>
			Comment( loi , $1 );
			push_const($1);
			C_los(EM_WSIZE).

	default		==>
				arg_error( "loi", $1).

C_los
	$1 == 4  ==>
	{
		reg_t a;
		reg_t b;
		reg_t c;
		int i;
		char *LD;
		arith size;
		const_str_t n;
		const_str_t size_str;

		Comment( los, $1);
		if (type_of_tos() == T_cst && top_const() <= MAX_UNROLL) {
			size = pop_const(size_str);
			if (size <= 4) {
				switch (size) {
				case 1: LD = "ldub"; break;
				case 2: LD = "lduh"; break;
				case 4: LD = "ld"; break;
				default: arg_error("C_los", size);
				}
				b = alloc_reg();
				if (type_of_tos() & T_reg2)
				{
					a= pop_reg_reg(&c);
					"$LD	[$a+$c], $b";
					free_reg(a);
					free_reg(c);
				}
				else
				{
					a = pop_reg_c13(n);
					"$LD	[$a+$n], $b";
					free_reg(a);
				}
				push_reg(b);
			} else if (size <= MAX_UNROLL) {    /* SUB-OPTIMAL */
				inc_tos(size-4);
				for (i = 0; i < size; i += 4) {
					b = alloc_reg();
					if (type_of_tos() & T_reg2)
					{
						a= pop_reg_reg(&c);
						"ld	[$a+$c], $b";
						push_reg(b);
						push_reg(a);
						inc_tos_reg(c);
					}
					else
					{
						a = pop_reg_c13(n);
						"ld	[$a+$n], $b";
						push_reg(b);
						if (n[0] == '-' || isdigit(n[0]))
						{
							push_reg(a);
							inc_tos(atoi(n));
						}
						else
						{
							b= alloc_reg();
							"add	$a, $n, $b";
							push_reg(b);
							free_reg(a);
						}
					}
					inc_tos(-4);
				}
				pop_nop(1);
			} else
				arg_error ("loi",  size);
		}
		else {
			a = pop_reg();	/* count */
			b = pop_reg();	/* addr */
			c = alloc_reg();
			flush_cache();
			"sub	$reg_sp, $a, $reg_sp"	/* HACK */
		"1:	 deccc	4, $a"
			"ld	[$b+$a], $c"
			"bnz	1b"
			"st	$c, [$reg_sp+$a]"	/* delay */
			free_reg(a);
			free_reg(b);
			free_reg(c);
		}
	}.
	default ==>
		arg_error("C_los", $1).


narg4(los)

C_ldl		==>
		Comment( ldl , $1 );
		{
			reg_t S1;
			reg_t S2;

			if (S1 = find_local($1, &S2)) {
				soft_alloc_reg(S1);
				soft_alloc_reg(S2);
				push_double_reg(S1);
			} else {
				soft_alloc_reg(reg_lb);
				push_reg(reg_lb);
				inc_tos($1);
				push_const(8);
				C_los(EM_WSIZE);
			}
		}.


C_lde..		==>
			Comment2( lde.. , $1, $2 );
			push_ext($1);
			inc_tos($2);
			push_const(8);
			C_los(EM_WSIZE).

C_ldf		==>
			Comment( ldf , $1 );
			inc_tos($1);
			push_const(8);
			C_los(EM_WSIZE).

C_lpi		==>
			Comment( lpi , $1 );
			push_ext($1).


/******************************************************************************/
/*                                                                            */
/*			Group 2 : Store instructions			      */
/*							 		      */
/******************************************************************************/

C_stl		==>
		Comment( stl , $1 );
		{
			reg_t S1;

			if ((S1 = find_local($1, NULL))) {
				pop_reg_as(S1);
			} else {
				soft_alloc_reg(reg_lb);
				push_reg(reg_lb);
				inc_tos($1);
				push_const(4);
				C_sts(EM_WSIZE);
			}
		}.

C_ste..		==>
			Comment2( ste.. , $1, $2 );
			push_ext($1);
			inc_tos($2);
			push_const(4);
			C_sts(EM_WSIZE).


C_sil		==>
		Comment( sil , $1 );
		{
			reg_t S1;
			reg_t S2;

			if (S1 = find_local($1, NULL)) {
				S2 = pop_reg();
				"st	$S2, [$S1]";
				free_reg(S2);
			} else {
				soft_alloc_reg(reg_lb);
				push_reg(reg_lb);
				inc_tos($1);
				push_const(4);
				C_los(EM_WSIZE);
				push_const(4);
				C_sts(EM_WSIZE);
			}
		}.

C_stf		==>
			Comment( stf , $1 );
			inc_tos($1);
			push_const(4);
			C_sts(EM_WSIZE).

C_sti
	( $1 == 1) ||
	( $1 == 2) ||
	( $1 % 4 == 0 )	==>
			Comment( sti, $1 );
			push_const($1);
			C_sts(EM_WSIZE).

	default		==>
				arg_error( "loi", $1).


C_sts
	$1 == 4  ==>
	{
		reg_t a;
		reg_t b;
		reg_t c;
		reg_t d;

		arith size;
		const_str_t n;
		const_str_t size_str;
		int i;
		char *ST;

		Comment( sts, $1);
		if (type_of_tos() == T_cst && top_const() <= MAX_UNROLL) {

			size = pop_const(size_str);
			if (size <= 4) {

				switch (size) {
				case 1: ST = "stb"; break;
				case 2: ST = "sth"; break;
				case 4: ST = "st"; break;
				default: arg_error("C_sti", size);
				}
				c= NULL;
				if (type_of_tos() & T_reg2)
					a= pop_reg_reg(&c);
				else
					a = pop_reg_c13(n);
				if (type_of_tos() == T_float)
					b= pop_float();
				else
					b = pop_reg();
				if (c)
				{
					"$ST	$b, [$a+$c]";
					free_reg(c);
				}
				else
					"$ST	$b, [$a+$n]";
				free_reg(a);
				free_reg(b);
			} else if (size <= MAX_UNROLL) {
				for (i = 0; i < size; i+=4) {
					c= NULL;
					if (type_of_tos() & T_reg2)
						a= pop_reg_reg(&c);
					else
						a = pop_reg_c13(n);
					if (type_of_tos() == T_float)
						b= pop_float();
					else
						b = pop_reg();
					if (c)
						"st	$b, [$a+$c]";
					else
						"st	$b, [$a+$n]";
					free_reg(b);
					if (c)
					{
						push_reg(a);
						inc_tos_reg(c);
					}
					else if (n[0] == '-' || isdigit(n[0]))
					{
						push_reg(a);
						inc_tos(atoi(n));
					}
					else
					{
						b= alloc_reg();
						"add	$a, $n, $b";
						push_reg(b);
						free_reg(a);
					}
					inc_tos(4);
				}
				pop_nop(1);
			} else
				arg_error ("sti", size);
		}
		else {
			force_alloc_output();
			d = pop_reg();		/* size */
			a = pop_reg();		/* address */
			flush_cache();
			b = alloc_reg();
			c = alloc_reg();
			"cmp	$d, 4";
			"bg,a	8f";
			"andcc	$d, 3, %g0";	/* delay slot */
			"be,a	4f";
			"ld	[$reg_sp], $b";	/* delay slot */
			"cmp	$d, 1";
			"be,a	1f";
			"ld	[$reg_sp], $b";	/* delay slot */
			"bl	0f";
			"cmp	$d, 2";
			"be	2f";
			"ld	[$reg_sp], $b";	/* delay slot */
		"3:	 set	EILLINS, %o0";
			"call	trp";
			"nop";
			"b	0f";
			"nop";
		"1:";
			"inc	STACK_CLICK, $reg_sp";
			"b	0f";
			"stb	$b, [$a]";	/* delay slot */
		"2:";
			"inc	STACK_CLICK, $reg_sp";
			"b	0f";
			"sth	$b, [$a]";	/* delay slot */
		"4:";
			"inc	STACK_CLICK, $reg_sp";
			"b	0f";
			"st	$b, [$a]";	/* delay slot */
		"8:";
			"bne	3b";
			"nop";
			"mov	$d, $b";
		"9:	 deccc	4, $b";
			"ld	[$reg_sp+$b], $c";
			"bnz	9b";
			"st	$c, [$a+$b]";	/* delay slot */
			"add	$reg_sp, $d, $reg_sp"	/* HACK */
		"0:"
			free_reg(a);
			free_reg(b);
			free_reg(c);
			free_reg(d);
			free_output();
		}
	}.
	default		==>
				arg_error( "sts", $1).

narg4(sts)

C_sdl		==>
		Comment( sdl , $1 );
		{
			reg_t S1;
			reg_t S2;

			S1 = find_local($1, NULL);
			if (S1) 
				pop_double_reg_as(S1);
			else {
				soft_alloc_reg(reg_lb);
				push_reg(reg_lb);
				inc_tos($1);
				push_const(8);
				C_sts(EM_WSIZE);
			}
		}.

C_sde..		==>
			Comment2( sde.. , $1, $2 );
			push_ext($1);
			inc_tos($2);
			push_const(8);
			C_sts(EM_WSIZE).

C_sdf		==>
			Comment( sdf , $1 );
			inc_tos($1);
			push_const(8);
			C_sts(EM_WSIZE).


/******************************************************************************/
/*									      */
/*		Group 3 : Integer arithmetic				      */
/*									      */
/******************************************************************************/


C_adi
	$1 == 4	==>
			Comment( adi , $1 );
			if ((type_of_tos()) == T_cst) {
				arith n;

				n = pop_const(NULL);
				inc_tos(n);
			} else {
				reg_t a;
				reg_t b;
				reg_t c;

				a = pop_reg();
				inc_tos_reg(a);
			}.
	default	==>
				arg_error( "adi", $1).

narg4(adi)

C_sbi
	$1 == 4	==>
			Comment( sbi , $1 );
			if ((type_of_tos()) == T_cst) {
				arith n;

				n = pop_const(NULL);
				inc_tos(-n);
			} else {
				reg_t a;
				reg_t b;
				reg_t c;

				a = pop_reg();
				b = pop_reg();
				c = alloc_reg();
				"sub	$b, $a, $c";
				free_reg(a);
				free_reg(b);
				push_reg(c);
			}.
	default	==>
				arg_error( "sbi", $1).

narg4(sbi)

C_mli
	$1 == 4	==>
	{
		unsigned int n0;
		unsigned int n1;
		reg_t orig;
		reg_t a;
		reg_t b;
		reg_t c;
		unsigned int n;
		const_str_t n_str;

		Comment( mli , $1 );

		if (type_of_tos() == T_cst) {
			n = pop_const(NULL);
			orig = pop_reg();
			c = reg_g0;
			while (n) {
				for (n0 = 0; !(n & 1); n>>=1)
					++n0;
				for (n1 = 0; n & 1; n>>=1)
					++n1;

				if (n0) {
					a = alloc_reg();
					sprint(n_str, "%d", n0);
					"sll	$orig, $n_str, $a";
					free_reg(orig);
					orig = a;
				}
				if (n1 == 1) {
					if (c == reg_g0) {
						soft_alloc_reg(orig);
						c = orig;
					} else {
						a = alloc_reg();
						"add	$c, $orig, $a";
						free_reg(c);
						c = a;
					}
					n <<= n1;
				} else {
					a = alloc_reg();
					sprint(n_str, "%d", n1);
					"sll	$orig, $n_str, $a";
					b = alloc_reg();
					"sub	$a, $orig, $b";
					free_reg(orig);
					orig = a;
					if (c == reg_g0)
						c = b;
					else {
						a = alloc_reg();
						"add	$c, $b, $a";
						free_reg(b);
						free_reg(c);
						c = a;
					}
				}
			}
			push_reg(c);
			free_reg(orig);
		} else {
			force_alloc_output();
			pop_reg_as(reg_o0);
			pop_reg_as(reg_o1);
			"call	mli4";
			"nop"			/* delay */
			free_output();
			forced_alloc_reg(O0);
			push_reg(O0);
		}
	}.
	default	==>
			arg_error( "mli", $1).

narg4(mli)

C_dvi
	$1 == 4	==>
	{
		reg_t a;
		reg_t b;
		int n;
		int n_exp;
		const_str_t n_exp_str;

		Comment( dvi , $1 );
#if MATH_DIVIDE
		if (type_of_tos() == T_cst &&
			power_of_2(top_const(), &n_exp))
		{
			sprint (n_exp_str, "%d", n_exp);
			n= pop_const(NULL);
			a= pop_reg();
			if (n <0)
			{
				b= alloc_reg();
				"neg	$a, $b";
				free_reg(a);
				a= b;
			}
			b= alloc_reg();
			"sra	$a, $n_exp_str, $b";
			free_reg(a);
			push_reg(b);
		}
		else
		{
			force_alloc_output();
			pop_reg_as(reg_o1);	/* denominator */
			pop_reg_as(reg_o0);	/* numerator */
			"call	dvi4";
			"nop"
			free_output();
			forced_alloc_reg(reg_o0);
			push_reg(reg_o0);
		}
#else
		not_implemented("dvi");
#endif /* MATH_DIVIDE */
	}.
	default	==>
			arg_error( "dvi", $1).

narg4(dvi)

C_rmi
	$1 == 4	==>
			Comment( rmi , $1 );
			{
				force_alloc_output();
				pop_reg_as(reg_o1);	/* denominator */
				pop_reg_as(reg_o0);	/* numerator */
				"call	dvi4";
				"nop"
				free_output();
				forced_alloc_reg(O1);
				push_reg(O1);
			}.
	default	==>
			arg_error( "rmi", $1).

narg4(rmi)

C_ngi
	$1 == 4	==>
			Comment( ngi , $1 );
			{
				reg_t a;
				reg_t b;

				a = pop_reg();
				b = alloc_reg();
				"sub	%g0, $a, $b";
				push_reg(b);
				free_reg(a);
			}.
	default	==>
			arg_error( "ngi", $1).

narg4(ngi)

C_sli
	$1 == 4	==>
			Comment( sli , $1 );
			{
				reg_t a;
				reg_t b;
				reg_t c;

				b = alloc_reg();
				if ((type_of_tos() == T_cst) &&
					(const13(top_const()))) {
					const_str_t n;

					pop_const(n);
					a = pop_reg();
					"sll	$a, $n, $b";
				} else {
					c = pop_reg();
					a = pop_reg();
					"sll	$a, $c, $b";
					free_reg(c);
				}
				free_reg(a);
				push_reg(b);
			}.
	default	==>
			arg_error( "sli", $1).

narg4(sli)

C_sri
	$1 == 4		==>
			Comment( sri , $1 );
			{
				reg_t a;
				reg_t b;
				reg_t c;

				b = alloc_reg();
				if ((type_of_tos() == T_cst) &&
					(const13(top_const()))) {
					const_str_t n;

					pop_const(n);
					a = pop_reg();
					"sra	$a, $n, $b";
				} else {
					c = pop_reg();
					a = pop_reg();
					"sra	$a, $c, $b";
					free_reg(c);
				}
				free_reg(a);
				push_reg(b);
			}.
	default	==>
			arg_error( "sri", $1).

narg4(sri)

/******************************************************************************/
/*									      */
/*		Group 4 : Unsigned arithmetic 				      */
/*									      */
/******************************************************************************/

C_adu		==>
			Comment( adu , $1 );
		 	C_adi( w).

narg4(adu)

C_sbu		==>
			Comment( sbu , $1 );
		 	C_sbi( w).

narg4(sbu)

C_mlu
	$1 == 4	==>
		Comment( mlu , $1 );
		C_mli($1).
/*
		{
			force_alloc_output();
			pop_reg_as(reg_o0);
			pop_reg_as(reg_o1);
			"call	mlu4";
			"nop"
			free_output();
			forced_alloc_reg(O0);
			push_reg(O0);
		}.
*/
	default	==>
			arg_error( "mlu", $1).

narg4(mlu)

C_dvu
	$1 == 4	==>
	{
		reg_t a;
		reg_t b;
		unsigned n;
		int n_exp;
		const_str_t n_exp_str;

		Comment( dvu , $1 );
		if (type_of_tos() == T_cst &&
			uns_power_of_2(top_const(), &n_exp))
		{
			sprint (n_exp_str, "%d", n_exp);
			n= pop_const(NULL);
			a= pop_reg();
			b= alloc_reg();
			"srl	$a, $n_exp_str, $b";
			free_reg(a);
			push_reg(b);
		}
		else
		{
			force_alloc_output();
			pop_reg_as(reg_o1);	/* denominator */
			pop_reg_as(reg_o0);	/* numerator */
			"call	dvu4";
			"nop"
			free_output();
			forced_alloc_reg(reg_o0);
			push_reg(reg_o0);
		}
	}.
	default	==>
			arg_error( "dvu", $1).

narg4(dvu)

C_rmu
	$1 == 4	==>
		Comment( rmu , $1 );
		{
			force_alloc_output();
			pop_reg_as(reg_o1);
			pop_reg_as(reg_o0);
			"call	dvu4";
			"nop"
			free_output();
			forced_alloc_reg(O1);
			push_reg(O1);
		}.
	default	==>
			arg_error( "rmu", $1).

narg4(rmu)

C_slu		==>
			Comment( slu , $1 );
			C_sli($1).

narg4(slu)

C_sru
	$1 == 4	==>
			{
				reg_t a;
				reg_t b;
				reg_t c;

				Comment( sru , $1 );
				b = alloc_reg();
				if ((type_of_tos() == T_cst) &&
					(const13(top_const()))) {
					const_str_t n;

					pop_const(n);
					a = pop_reg();
					"srl	$a, $n, $b";
				} else {
					c = pop_reg();
					a = pop_reg();
					"srl	$a, $c, $b";
					free_reg(c);
				}
				free_reg(a);
				push_reg(b);
			}.
	default	==>
			arg_error( "sru", $1).

narg4(sru)

/******************************************************************************/
/*									      */
/*		Group 5 : Floating point arithmetic 			      */
/*									      */
/******************************************************************************/

C_adf		==>
	{
		Comment( adf, $1);
		push_const($1);
		C_adf_narg();
	}.

C_adf_narg	==>
	{	
		reg_t f1;
		reg_t f2;
		reg_t f3;
		int n;

		Comment0( adf_narg);
		if (type_of_tos() == T_cst)
		{
			n= pop_const(NULL);
			if (n == EM_WSIZE)
			{
				f1= pop_float();
				inc_tos_reg(f1);
			}
			else if (n == EM_DSIZE)
			{
				f1= pop_double(NULL);
				f2= pop_double(NULL);
				f3= alloc_double(NULL);
				"faddd	$f1, $f2, $f3";
				free_double_reg(f1);
				free_double_reg(f2);
				push_double_reg(f3);
			}
			else
				arg_error ("unimp adf", n);
		}
		else
			not_implemented ("adf_narg");
	}.

C_sbf		==>
	{
		Comment( sbf, $1);
		push_const($1);
		C_sbf_narg();
	}.

C_sbf_narg	==>
	{	
		reg_t f1;
		reg_t f2;
		reg_t f3;
		int n;

		Comment0( sbf_narg);
		if (type_of_tos() == T_cst)
		{
			n= pop_const(NULL);
			if (n == EM_WSIZE)
			{
				f1= pop_float();
				f2= pop_float();
				f3= alloc_float();
				"fsubs	$f2, $f1, $f3";
				free_reg(f1);
				free_reg(f2);
				push_reg(f3);
			}
			else if (n == EM_DSIZE)
			{
				f1= pop_double(NULL);
				f2= pop_double(NULL);
				f3= alloc_double(NULL);
				"fsubd	$f2, $f1, $f3";
				free_double_reg(f1);
				free_double_reg(f2);
				push_double_reg(f3);
			}
			else
				arg_error ("unimp sbf", n);
		}
		else
			not_implemented ("sbf_narg");
	}.

C_mlf		==>
	{
		Comment( mlf, $1);
		push_const($1);
		C_mlf_narg();
	}.

C_mlf_narg	==>
	{	
		reg_t f1;
		reg_t f2;
		reg_t f3;
		int n;

		Comment0( mlf_narg);
		if (type_of_tos() == T_cst)
		{
			n= pop_const(NULL);
			if (n == EM_WSIZE)
			{
				f1= pop_float();
				f2= pop_float();
				f3= alloc_float();
				"fmuls	$f2, $f1, $f3";
				free_reg(f1);
				free_reg(f2);
				push_reg(f3);
			}
			else if (n == EM_DSIZE)
			{
				f1= pop_double(NULL);
				f2= pop_double(NULL);
				f3= alloc_double(NULL);
				"fmuld	$f2, $f1, $f3";
				free_double_reg(f1);
				free_double_reg(f2);
				push_double_reg(f3);
			}
			else
				arg_error ("unimp mlf", n);
		}
		else
			not_implemented ("mlf_narg");
	}.

C_dvf		==>
	{
		Comment( dvf, $1);
		push_const($1);
		C_dvf_narg();
	}.

C_dvf_narg	==>
	{	
		reg_t f1;
		reg_t f2;
		reg_t f3;
		int n;

		Comment0( dvf_narg);
		if (type_of_tos() == T_cst)
		{
			n= pop_const(NULL);
			if (n == EM_WSIZE)
			{
				f1= pop_float();
				f2= pop_float();
				f3= alloc_float();
				"fdivs	$f2, $f1, $f3";
				free_reg(f1);
				free_reg(f2);
				push_reg(f3);
			}
			else if (n == EM_DSIZE)
			{
				f1= pop_double(NULL);
				f2= pop_double(NULL);
				f3= alloc_double(NULL);
				"fdivd	$f2, $f1, $f3";
				free_double_reg(f1);
				free_double_reg(f2);
				push_double_reg(f3);
			}
			else
				arg_error ("unimp dvf", n);
		}
		else
			not_implemented ("dvf_narg");
	}.

C_ngf		==>
	{
		Comment( ngf, $1);
		push_const($1);
		C_ngf_narg();
	}.

C_ngf_narg	==>
	{	
		reg_t f1;
		reg_t f2;
		int n;

		Comment0( ngf_narg);
		if (type_of_tos() == T_cst)
		{
			n= pop_const(NULL);
			if (n == EM_WSIZE || n == EM_DSIZE)
			{
				f1= pop_float();
				f2= alloc_float();
				"fnegs	$f1, $f2";
				free_reg(f1);
				push_reg(f2);
			}
			else
				arg_error ("unimp ngf", n);
		}
		else
			not_implemented ("ngf_narg");
	}.

C_fif		==>
	Comment( fif, $1);
	push_const($1);
	C_fif_narg().

C_fif_narg	==>
	{
		int n;
		reg_t a;
		reg_t b;
		reg_t c;
		reg_t d;

		Comment0( fif_narg );
		if (type_of_tos() == T_cst)
		{
			n= pop_const(NULL);

			if (n==4)
			{
				"! unimplemented fif 4";
				"st	%g0, [%g0]"; /* unimp */
			}
			else if (n==8)
			{
				flush_cache();
				"call	fif8";
				"nop";
			}
			else
				arg_error ("fif", n);
		}
		else
		{
			a= alloc_reg();
			flush_cache();
			force_alloc_output();
			b= alloc_reg();
			c= alloc_reg();
			d= pop_reg();
			"cmp	8, $d";
			"be	8f";
			"nop";
			"cmp	4, $d";
			"bne	0f";
			"nop";
		"4:";
			"! unimplemented fif 4";
			"st	%g0, [%g0]";
			"b	1f";
		"0:";
			"set	EILLINS, $reg_o0";
			"call	trp";
			"nop";
			"b	1f";
		"8:";
			"call	fif8";
			"nop";
		"1:";
			free_reg(a);
			free_reg(b);
			free_reg(c);
			free_reg(d);
			free_output();
		}
	}.


C_fef		==>
	Comment( fef, $1);
	push_const($1);
	C_fef_narg().

C_fef_narg	==>
	{
		int n;
		reg_t a;
		reg_t b;
		reg_t c;
		reg_t d;
		reg_t e;

		Comment0( fef_narg );
		if (type_of_tos() == T_cst)
		{
			n= pop_const(NULL);

			if (n==4)
			{
				"! unimplemented fef 4";
				"st	%g0, [%g0]"; /* unimp */
			}
			else if (n==8)
			{
				flush_cache();
				"call	fef8";
				"nop";
			}
			else
				arg_error ("fef", n);
		}
		else
		{
			a= alloc_reg();
			flush_cache();
			force_alloc_output();
			b= alloc_reg();
			c= alloc_reg();
			d= pop_reg();
			"cmp	8, $d";
			"be	8f";
			"nop";
			"cmp	4, $d";
			"bne	0f";
			"nop";
		"4:";
			"! unimplemented fef 4";
			"st	%g0, [%g0]";
			"b	1f";
		"0:";
			"set	EILLINS, $reg_o0";
			"call	trp";
			"nop";
			"b	1f";
		"8:";
			"call	fef8";
			"nop";
		"1:";
			free_reg(a);
			free_reg(b);
			free_reg(c);
			free_reg(d);
			free_output();
		}
	}.

/******************************************************************************/
/*									      */
/*		Group 6 : Pointer arithmetic 				      */
/*									      */
/******************************************************************************/

C_adp		==>
			Comment( adp , $1 );
			inc_tos($1).

C_ads
	$1 == 4	==>
			Comment( ads , $1 );
			if ((type_of_tos()) == T_cst) {
				arith n;

				n = pop_const(NULL);
				inc_tos(n);
			} else {
				reg_t a;
				reg_t b;
				reg_t c;

				a = pop_reg();
				inc_tos_reg(a);
			}.
	default		==>
				arg_error( "ads", $1).

narg4(ads)

C_sbs
	$1 == 4	==>
			Comment( sbs , $1 );

			if ((type_of_tos()) == T_cst) {
				arith n;

				n = pop_const(NULL);
				inc_tos(-n);
			} else {
				reg_t a;
				reg_t b;
				reg_t c;

				a = pop_reg();
				b = pop_reg();
				c = alloc_reg();
				"sub	$b, $a, $c";
				free_reg(a);
				free_reg(b);
				push_reg(c);
			}.
	default	==>
			arg_error( "sbs", $1).

narg4(sbs)

/******************************************************************************/
/*									      */
/*		Group 7 : Increment/decrement/zero			      */
/*									      */
/******************************************************************************/

C_inc		==>
			Comment0( inc  );
			inc_tos(1).

C_inl		==>
		Comment( inl , $1 );
		{
			reg_t S1;

			if (S1 = find_local($1, NULL)) {
				change_reg(S1);
				"inc	1, $S1";
			} else {
				soft_alloc_reg(reg_lb);
				push_reg(reg_lb);
				inc_tos($1);
				C_loi(4);
				C_inc();
				soft_alloc_reg(reg_lb);
				push_reg(reg_lb);
				inc_tos($1);
				C_sti(4);
			}
		}.

C_ine..		==>
		{ 
			char *ename;
			const_str_t evalue;
			reg_t a;
			reg_t b;

			Comment2( ine.. , $1, $2 );
			a= alloc_reg();
			b= alloc_reg();

			ename= $1;
			sprint(evalue, "%d", $2);
			"sethi	%hi($ename+$evalue), $a";
			"ld	[$a+%lo($ename+$evalue)], $b";
			"inc	$b";
			"st	$b, [$a+%lo($ename+$evalue)]"
			free_reg(a);
			free_reg(b);
		}.


C_dec		==>
			Comment0( dec );
			inc_tos(-1).

C_del		==>
		Comment( del , $1 );
		{
			reg_t S1;

			if (S1 = find_local($1, NULL)) {
				change_reg(S1);
				"dec	1, $S1";
			} else {
				soft_alloc_reg(reg_lb);
				push_reg(reg_lb);
				inc_tos($1);
				C_loi(4);
				C_dec();
				soft_alloc_reg(reg_lb);
				push_reg(reg_lb);
				inc_tos($1);
				C_sti(4);
			}
		}.

C_dee..		==>
		{ 
			char *ename;
			const_str_t evalue;
			reg_t a;
			reg_t b;

			Comment2( dee.. , $1, $2 );
			a= alloc_reg();
			b= alloc_reg();

			ename= $1;
			sprint(evalue, "%d", $2);
			"sethi	%hi($ename+$evalue), $a";
			"ld	[$a+%lo($ename+$evalue)], $b";
			"dec	$b";
			"st	$b, [$a+%lo($ename+$evalue)]"
			free_reg(a);
			free_reg(b);
		}.

C_zrl		==>
		Comment( zrl , $1 );
		{
			reg_t S1;

			if (S1 = find_local($1, NULL)) {
				change_reg(S1);
				"mov	0, $S1";
			} else {
				push_const(0);
				soft_alloc_reg(reg_lb);
				push_reg(reg_lb);
				inc_tos($1);
				C_sti(4);
			}
		}.

C_zre..		==>
		{ 
			char *ename;
			const_str_t evalue;
			reg_t a;

			Comment2( zre.. , $1, $2 );
			a= alloc_reg();

			ename= $1;
			sprint(evalue, "%d", $2);
			"sethi	%hi($ename+$evalue), $a";
			"st	%g0, [$a+%lo($ename+$evalue)]"
			free_reg(a);
		}.

C_zrf 		==>
			Comment( zrf , $1 );
			push_const($1);
			C_zrf_narg().

C_zrf_narg	==>
			Comment0( zrf_narg );
			C_zer_narg().


C_zer		==>
			Comment( zer, $1);
			push_const($1);
			C_zer_narg().

C_zer_narg	==>
	{
		reg_t a;
		int n;
		const_str_t n_str;

		Comment0( zer_narg);

		if (type_of_tos() == T_cst && top_const() <= 8)
		{
			n= pop_const(n_str);
			if (n == 4)
				push_const(0);
			else if (n == 8)
			{
				push_const(0);
				push_const(0);
			}
			else
				arg_error ("zer", n);
		}
		else
		{
			a= pop_reg();
			flush_cache();
			"sub	$reg_sp, $a, $reg_sp";
		"1:"
			"deccc	4, $a";		/* hack */
			"st	%g0, [$reg_sp+$a]";
			"bne	1b";
			"nop";
			free_reg(a);
		}
	}.

/******************************************************************************/
/*									      */
/*		Group 8 : Convert 					      */
/*									      */
/******************************************************************************/

/* cii, ciu, cuu, cui are assumed to be called with legal arguments only */

C_cii		==>
	{
		reg_t a;	/* target obj size */
		reg_t b;	/* src obj size */
		int n1;		/* target obj size */
		int n2; 	/* src obj size */
		const_str_t n1_str;

		Comment0( cii );
		a= NULL;
		b= NULL;

		if (type_of_tos() != T_cst)
		{
			a= pop_reg();
			b= pop_reg();
		}
		else
		{
			n1= pop_const(n1_str);
			if (type_of_tos() != T_cst)
			{
				a= alloc_reg();
				"set	$n1_str, $a";
				b= pop_reg();
			}
			else
				n2= pop_const(NULL);
		}

		if (!a)
		{
			a = pop_reg();
			if (n1 > EM_WSIZE)
				arg_error ("unimp cii", n1);
			if (n2 > EM_WSIZE)
				arg_error ("unimp cii", n2);
			if (n2 < EM_WSIZE) {
				b = alloc_reg();
				if (n2 == 1)
				{
					"sll	$a, 24, $b";
					"sra	$b, 24, $b";
				}
				else if (n2 == 2)
				{
					"sll	$a, 16, $b";
					"sra	$b, 16, $b";
				}
				free_reg(a);
				push_reg(b);
			}
			else
				push_reg(a);
		} else {
			flush_cache();
			"cmp	$a, $b";
			"ble	4f";
			"nop";			/* delay slot */
			"cmp	$b, 1";
			"bne	2f";
			"nop";			/* delay slot */
		"1:";
			"b	3f";
			"ldsb	[$reg_sp+3], $a";	/* delay slot */
		"2:"
			"ldsh	[$reg_sp+2], $a";
		"3:";
			"st	$a, [$reg_sp]";
		"4:";
			free_reg(a);
			free_reg(b);
		}
	}.


C_cuu		==>
			Comment0( cuu  );
		 	pop_nop(2).

C_ciu		==>
			Comment0( ciu );
		 	pop_nop(2).

C_cui		==>
			Comment0( cui );
		 	pop_nop(2).

C_cfi		==>
	{
		reg_t a;	/* target (int) size */
		reg_t b;	/* src (float) size */
		reg_t s1;
		reg_t s2;
		reg_t d1;
		reg_t d2;
		int n1;		/* target (int) size */
		int n2;		/* src (float) size */
		const_str_t n1_str;

		Comment0( cfi );
		a= NULL;
		b= NULL;
		if (type_of_tos() != T_cst)
		{
			a= pop_reg();
			b= pop_reg();
		}
		else
		{
			n1= pop_const (n1_str);
			if (type_of_tos() != T_cst)
			{
				a= alloc_reg();
				"set	$n1_str, $a";
				b= pop_reg();
			}
			else
				n2= pop_const(NULL);
		}

		if (!a)
		{
			if (n1 != EM_WSIZE)
				arg_error ("unimp cfi", n1);
			if (n2 == EM_WSIZE)
			{
				s1= pop_float();
				d1= alloc_float();
				"fstoi	$s1, $d1";
				free_reg(s1);
				push_reg(d1);
			}
			else if (n2 == EM_DSIZE)
			{
				s1= pop_double(NULL);
				d1= alloc_float();
				"fdtoi	$s1, $d1";
				free_double_reg(s1);
				push_reg(d1);
			}
			else
				arg_error ("unimp cfi", n2);
		}
		else
		{

			d1= alloc_float();
			flush_cache();
			force_alloc_output();
			"cmp	$a, 4";
			"bne	0f";
			"nop";
			"cmp	$b, 4";
			"be	4f";
			"nop";
			"cmp	$b, 8";
			"bne	0f";
			"nop";
		"8:";
			"ld	[$reg_sp], %f0";
			"ld	[$reg_sp+STACK_CLICK], %f1";
			"fdtoi	%f0, $d1";
			"b	1f";
			"inc	2*STACK_CLICK, $reg_sp"; /* delay slot */
		"4:";
			"ld	[$reg_sp+2*STACK_CLICK], %f0";
			"fstoi	%f0, $d1";
			"b	1f";
			"inc	STACK_CLICK, $reg_sp"; /* delay slot */
		"0:";
			"set	E_EM_CFI, %o0";
			"call	trp";
			"nop";
		"1:";
			free_reg(a);
			free_reg(b);
			push_reg(d1);
			free_output();
		}
	}.

C_cfu		==>
			Comment0( cfu );
			C_cfi().

C_cff		==>
	{
		reg_t a;	/* target (int) size */
		reg_t b;	/* src (float) size */
		int n1;		/* target (int) size */
		int n2;		/* src (float) size */
		const_str_t n1_str;

		Comment0( cff );
		a= NULL;
		b= NULL;
		if (type_of_tos() != T_cst)
		{
			a= pop_reg();
			b= pop_reg();
		}
		else
		{
			n1= pop_const (n1_str);
			if (type_of_tos() != T_cst)
			{
				a= alloc_reg();
				"set	$n1_str, $a";
				b= pop_reg();
			}
			else
				n2= pop_const(NULL);
		}

		if (!a)
		{
			if (n1 == EM_WSIZE)
			{
				if (n2 == EM_DSIZE)
				{
					a= pop_double(NULL);
					b= alloc_float();
					"fdtos	$a, $b";
					free_double_reg(a);
					push_reg(b);
				} else if (n2 != EM_WSIZE)
					arg_error ("unimp cff", n2);
			}
			else if (n1 == EM_DSIZE)
			{
				if (n2 == EM_WSIZE)
				{
					a= pop_float();
					b= alloc_double(NULL);
					"fstod	$a, $b";
					free_reg(a);
					push_double_reg(b);
				} else if (n2 != EM_DSIZE)
					arg_error ("unimp cff", n2);
			}
			else
				arg_error ("unimp cff", n1);
		}
		else
		{

			flush_cache();
			force_alloc_output();
			"cmp	$b, $a";
			"be	1f";
			"nop";		/* delay slot */
			"cmp	$b, 4";
			"be	4f";
			"nop";
			"cmp	$b, 8";
			"be	8f";
			"nop";
		"0:"
			"set	E_EM_CFF, %o0";
			"call	trp";
			"nop";
		"4:";
			"cmp	$a, 8";
			"bne	0b";
			"nop";
			"ld	[$reg_sp], %f0";
			"fstod	%f0, %f2";
			"dec	STACK_CLICK, $reg_sp";
			"st	%f2, [$reg_sp]";
			"st	%f3, [$reg_sp+STACK_CLICK]";
			"b	1f";
			"nop";
		"8:";
			"cmp	$a, 4";
			"bne	0b";
			"nop";
			"ld	[$reg_sp], %f0";
			"ld	[$reg_sp+STACK_CLICK], %f1";
			"fdtos	%f0, %f2";
			"inc	STACK_CLICK, $reg_sp";
			"st	%f2, [$reg_sp]";
		"1:";
			free_reg(a);
			free_reg(b);
			free_output();
		}
	}.

C_cif		==>
	{
		reg_t a;	/* target (float) size */
		reg_t b;	/* src (int) size */
		int n1;		/* target (float) size */
		int n2;		/* src (int) size */
		reg_t r1;
		reg_t f1;
		const_str_t n1_str;

		Comment0( cif );
		a= NULL;
		b= NULL;
		if (type_of_tos() != T_cst)
		{
			a= pop_reg();
			b= pop_reg();
		}
		else
		{
			n1= pop_const (n1_str);
			if (type_of_tos() != T_cst)
			{
				a= alloc_reg();
				"set	$n1_str, $a";
				b= pop_reg();
			}
			else
				n2= pop_const(NULL);
		}

		if (!a)
		{
			if (n2 != EM_WSIZE)
				arg_error ("unimp cif", n2);
			else
			{
				if (n1 == EM_WSIZE)
				{
					r1= pop_float();
					f1= alloc_float();
					"fitos	$r1, $f1";
					free_reg(r1);
					push_reg(f1);

				}
				else if (n1 == EM_DSIZE)
				{
					r1= pop_float();
					f1= alloc_double(NULL);
					"fitod	$r1, $f1";
					free_reg(r1);
					push_double_reg(f1);
				}
				else
					arg_error ("unimp cif", n1);
			}
		}
		else
		{
			flush_cache();
			force_alloc_output();
			"cmp	$a, 4";
			"be	4f";
			"nop";		/* delay slot */
			"cmp	$a, 8";
			"be	8f";
			"nop";		/* delay slot */
		"1:"
			"set	E_EM_CUF, %o0";
			"call	trp";
			"nop";
		"4:";
			"cmp	$b, 4";
			"bne	1b";
			"nop";		/* delay slot */
			"ld	[$reg_sp], %f0";
			"fitos	%f0, %f1";
			"b	0f";
			"st	%f1, [$reg_sp]";	/* delay slot */
		"8:";
			"dec	STACK_CLICK, $reg_sp";
			"cmp	$b, 4";
			"bne	1b";
			"nop";		/* delay slot */
			"ld	[$reg_sp+STACK_CLICK], %f0";
			"fitod	%f0, %f2";
			"st	%f2, [$reg_sp]";
			"b	0f";
			"st	%f3, [$reg_sp+STACK_CLICK]"; /* delay slot */
		"0:";
			free_reg(a);
			free_reg(b);
			free_output();
		}
	}.


C_cuf		==>
	{
		reg_t a;	/* target (float) size */
		reg_t b;	/* src (int) size */
		reg_t c;
		reg_t fs1;
		reg_t fs2;
		reg_t fd1;
		reg_t fd2;
		int n1;		/* target (float) size */
		int n2;		/* src (int) size */
		const_str_t n1_str;

		Comment0( cuf );
		a= NULL;
		b= NULL;
		if (type_of_tos() != T_cst)
		{
			a= pop_reg();
			b= pop_reg();
		}
		else
		{
			n1= pop_const (n1_str);
			if (type_of_tos() != T_cst)
			{
				a= alloc_reg();
				"set	$n1_str, $a";
				b= pop_reg();
			}
			else
				n2= pop_const(NULL);
		}

		if (!a)
		{
			if (n2 != EM_WSIZE)
				arg_error ("unimp cuf", n2);
			else
			{
				if (n1 == EM_WSIZE)
				{
					fs1= pop_float();
					fs2= alloc_float();
					a= alloc_reg();
					"fitos	$fs1, $fs2";
					"sethi	%hi(Fs0), $a";
					"ld	[$a+%lo(Fs0)], $fs1";
					"fcmpes	$fs2, $fs1";
					"nop";
					"fbge	0f";
					"nop";
					"sethi	%hi(Fs80000000), $a";
					"ld	[$a+%lo(Fs80000000)], $fs1";
					"fadds	$fs1, $fs2, $fs2";
				"0:";
					push_reg(fs2);
					free_reg(fs1);
					free_reg(a);
				}
				else if (n1 == EM_DSIZE)
				{
					fs1= pop_float();
					fd1= alloc_double(NULL);
					fd2= alloc_double(NULL);
					a= alloc_reg();
					"fitod	$fs1, $fd2";
					"sethi	%hi(Fd0), $a";
					"ldd	[$a+%lo(Fd0)], $fd1";
					"fcmped	$fd2, $fd1";
					"nop";
					"fbge	0f";
					"nop";
					"sethi	%hi(Fd80000000), $a";
					"ldd	[$a+%lo(Fd80000000)], $fd1";
					"faddd	$fd1, $fd2, $fd2";
				"0:";
					free_reg(fs1);
					free_double_reg(fd1);
					push_double_reg(fd2);
					free_reg(a);
				}
				else
					arg_error ("unimp cuf", n1);
			}
		}
		else
		{
#if 0
			flush_cache();

			"cmp	$a, 4";
			"be	4f";
			"nop";		/* delay slot */
			"cmp	$a, 8";
			"be	8f";
			"nop";		/* delay slot */
		"1:"
			"set	E_EM_CUF, %o0";
			"set	fatal, %g1";
			"jmp	%g1";
			"nop";
		"4:";
			"cmp	$b, 4";
			"bne	1b";
			"nop";		/* delay slot */
			"ld	[$reg_sp], $c";
			"tst	$c";
			"bl	5f";
			"nop";		/* delay slot */
			"ld	[$reg_sp], %f0";
			"fitos	%f0, %f1";
			"b	0f";
			"st	%f1, [$reg_sp]";	/* delay slot */
		"5:";
			"set	MAX_INT, $b";
			"sub	$c, $b, $a";
			"st	$a, [$reg_sp]";
			"ld	[$reg_sp], %f0";
			"st	$b, [$reg_sp]";
			"ld	[$reg_sp], %f1";
			"fitos	%f0, %f2";
			"fitos	%f1, %f3";
			"fadds	%f2, %f3, %f0";
			"b	0f";
			"st	%f0, [$reg_sp]";		/* delay slot */
		"8:";
			"dec	STACK_CLICK, $reg_sp";
			"cmp	$b, 4";
			"bne	1b";
			"nop";		/* delay slot */
			"ld	[$reg_sp+STACK_CLICK], $c";
			"tst	$c";
			"bl	9f";
			"nop";		/* delay slot */
			"ld	[$reg_sp+STACK_CLICK], %f0";
			"fitod	%f0, %f2";
			"st	%f2, [$reg_sp]";
			"b	0f";
			"st	%f3, [$reg_sp+STACK_CLICK]";	/* delay slot */
		"9:";
			"set	MAX_INT, $b";
			"sub	$c, $b, $a";
			"st	$a, [$reg_sp+STACK_CLICK]";
			"ld	[$reg_sp+STACK_CLICK], %f0";
			"st	$b, [$reg_sp+STACK_CLICK]";
			"ld	[$reg_sp+STACK_CLICK], %f1";
			"fitod	%f0, %f2";
			"fitod	%f1, %f4";
			"fadds	%f2, %f4, %f0";
			"st	%f0, [$reg_sp]";
			"b	0f";
			"st	%f1, [$reg_sp+STACK_CLICK]";	/* delay slot */
		"0:";
			free_reg(a);
			free_reg(b);
			free_reg(c);
#else
	not_implemented ("cuf");
#endif
		}
	}.
/******************************************************************************/
/*									      */
/*		Group 9 : Logical 	 				      */
/*									      */
/******************************************************************************/

C_and		==>
	Comment( and, $1);
	push_const($1);
	C_and_narg().

C_and_narg	==>
	{
		reg_t a;
		reg_t b;
		reg_t c;
		reg_t d;
		reg_t e;
		reg_t f;
		const_str_t a_cst_str;
		const_str_t b_cst_str;
		const_str_t c_cst_str;
		const_str_t d_cst_str;
		int n;
		const_str_t n_str;

		Comment0( and_narg );
		if (type_of_tos() == T_cst)
		{
			n= pop_const(n_str);
			if (n == EM_WSIZE)
			{
				if (type_of_tos() == T_cst && const13(top_const()))
				{
					a= NULL;
					pop_const (a_cst_str);
				}
				else
					a= pop_reg();
				if (type_of_tos() == T_cst && const13(top_const()))
				{
					b= NULL;
					pop_const (b_cst_str);
				}
				else
					b= pop_reg();
				if (!a && !b)
				{
					a= alloc_reg();
					"mov	$a_cst_str, $a";	
				}
				c= alloc_reg();
				if (a)
					if (b)
						"and	$a, $b, $c";
					else
						"and	$a, $b_cst_str, $c";
				else
					"and	$b, $a_cst_str, $c";
				free_reg(a);
				free_reg(b);
				push_reg(c);
			}
			else if (n == EM_DSIZE)
			{
				if (type_of_tos() == T_cst && const13(top_const()))
				{
					a= NULL;
					pop_const (a_cst_str);
				}
				else
					a= pop_reg();
				if (type_of_tos() == T_cst && const13(top_const()))
				{
					b= NULL;
					pop_const (b_cst_str);
				}
				else
					b= pop_reg();
				if (type_of_tos() == T_cst && const13(top_const()))
				{
					c= NULL;
					pop_const (c_cst_str);
				}
				else
					c= pop_reg();
				if (type_of_tos() == T_cst && const13(top_const()))
				{
					d= NULL;
					pop_const (d_cst_str);
				}
				else
					d= pop_reg();
				if (!b && !d)
				{
					b= alloc_reg();
					"mov	$b_cst_str, $b";	
				}
				e= alloc_reg();
				if (b)
					if (d)
						"and	$b, $d, $e";
					else
						"and	$b, $d_cst_str, $e";
				else
					"and	$d, $b_cst_str, $e";
				free_reg(b);
				free_reg(d);
				push_reg(e);
				if (!a && !c)
				{
					a= alloc_reg();
					"mov	$a_cst_str, $a";	
				}
				e= alloc_reg();
				if (a)
					if (c)
						"and	$a, $c, $e";
					else
						"and	$a, $c_cst_str, $e";
				else
					"and	$c, $a_cst_str, $e";
				free_reg(a);
				free_reg(c);
				push_reg(e);
			}
			else if (!(n % EM_WSIZE))
			{
				a= alloc_reg();
				b= alloc_reg();
				c= alloc_reg();
				d= alloc_reg();
				e= alloc_reg();
				f= alloc_reg();
				flush_cache();

				"set	$n_str, $a";
				"add	$reg_sp, $a, $b";
				"mov	$a, $c";
			"1:";
				"deccc	4, $c";
				"ld	[$reg_sp+$c], $d";
				"ld	[$b+$c], $e";
				"and	$d, $e, $f";
				"bnz	1b";
				"st	$f, [$b+$c]";	/* delay slot */
				"add	$reg_sp, $a, $reg_sp";
				free_reg(a);
				free_reg(b);
				free_reg(c);
				free_reg(d);
				free_reg(e);
				free_reg(f);
			}
			else
				arg_error ("unimp and", n);
		}
		else
		{
			a= pop_reg();
			b= alloc_reg();
			c= alloc_reg();
			d= alloc_reg();
			e= alloc_reg();
			f= alloc_reg();
			flush_cache();

			"add	$reg_sp, $a, $b";
			"mov	$a, $c";
		"1:";
			"deccc	4, $c";
			"ld	[$reg_sp+$c], $d";
			"ld	[$b+$c], $e";
			"and	$d, $e, $f";
			"bnz	1b";
			"st	$f, [$b+$c]";	/* delay slot */
			"add	$reg_sp, $a, $reg_sp";
			free_reg(a);
			free_reg(b);
			free_reg(c);
			free_reg(d);
			free_reg(e);
			free_reg(f);
			
		}
	}.

C_ior		==>
	Comment( ior, $1);
	push_const($1);
	C_ior_narg().

C_ior_narg	==>
	{
		reg_t a;
		reg_t b;
		reg_t c;
		reg_t d;
		reg_t e;
		reg_t f;
		const_str_t a_cst_str;
		const_str_t b_cst_str;
		const_str_t c_cst_str;
		const_str_t d_cst_str;
		int n;
		const_str_t n_str;

		Comment0( ior_narg );
		if (type_of_tos() == T_cst)
		{
			n= pop_const(n_str);
			if (n == EM_WSIZE)
			{
				if (type_of_tos() == T_cst && const13(top_const()))
				{
					a= NULL;
					pop_const (a_cst_str);
				}
				else
					a= pop_reg();
				if (type_of_tos() == T_cst && const13(top_const()))
				{
					b= NULL;
					pop_const (b_cst_str);
				}
				else
					b= pop_reg();
				if (!a && !b)
				{
					a= alloc_reg();
					"mov	$a_cst_str, $a";	
				}
				c= alloc_reg();
				if (a)
					if (b)
						"or	$a, $b, $c";
					else
						"or	$a, $b_cst_str, $c";
				else
					"or	$b, $a_cst_str, $c";
				free_reg(a);
				free_reg(b);
				push_reg(c);
			}
			else if (n == EM_DSIZE)
			{
				if (type_of_tos() == T_cst && const13(top_const()))
				{
					a= NULL;
					pop_const (a_cst_str);
				}
				else
					a= pop_reg();
				if (type_of_tos() == T_cst && const13(top_const()))
				{
					b= NULL;
					pop_const (b_cst_str);
				}
				else
					b= pop_reg();
				if (type_of_tos() == T_cst && const13(top_const()))
				{
					c= NULL;
					pop_const (c_cst_str);
				}
				else
					c= pop_reg();
				if (type_of_tos() == T_cst && const13(top_const()))
				{
					d= NULL;
					pop_const (d_cst_str);
				}
				else
					d= pop_reg();
				if (!b && !d)
				{
					b= alloc_reg();
					"mov	$b_cst_str, $b";	
				}
				e= alloc_reg();
				if (b)
					if (d)
						"or	$b, $d, $e";
					else
						"or	$b, $d_cst_str, $e";
				else
					"or	$d, $b_cst_str, $e";
				free_reg(b);
				free_reg(d);
				push_reg(e);
				if (!a && !c)
				{
					a= alloc_reg();
					"mov	$a_cst_str, $a";	
				}
				e= alloc_reg();
				if (a)
					if (c)
						"or	$a, $c, $e";
					else
						"or	$a, $c_cst_str, $e";
				else
					"or	$c, $a_cst_str, $e";
				free_reg(a);
				free_reg(c);
				push_reg(e);
			}
			else if (!(n % EM_WSIZE))
			{
				a= alloc_reg();
				b= alloc_reg();
				c= alloc_reg();
				d= alloc_reg();
				e= alloc_reg();
				f= alloc_reg();
				flush_cache();

				"set	$n_str, $a";
				"add	$reg_sp, $a, $b";
				"mov	$a, $c";
			"1:";
				"deccc	4, $c";
				"ld	[$reg_sp+$c], $d";
				"ld	[$b+$c], $e";
				"or	$d, $e, $f";
				"bnz	1b";
				"st	$f, [$b+$c]";	/* delay slot */
				"add	$reg_sp, $a, $reg_sp";
				free_reg(a);
				free_reg(b);
				free_reg(c);
				free_reg(d);
				free_reg(e);
				free_reg(f);
			}
			else
				arg_error ("unimp ior", n);
		}
		else
		{
			a= pop_reg();
			b= alloc_reg();
			c= alloc_reg();
			d= alloc_reg();
			e= alloc_reg();
			f= alloc_reg();
			flush_cache();

			"add	$reg_sp, $a, $b";
			"mov	$a, $c";
		"1:";
			"deccc	4, $c";
			"ld	[$reg_sp+$c], $d";
			"ld	[$b+$c], $e";
			"or	$d, $e, $f";
			"bnz	1b";
			"st	$f, [$b+$c]";	/* delay slot */
			"add	$reg_sp, $a, $reg_sp";
			free_reg(a);
			free_reg(b);
			free_reg(c);
			free_reg(d);
			free_reg(e);
			free_reg(f);
		}
	}.



C_xor		==>
	Comment( xor, $1);
	push_const($1);
	C_xor_narg().

C_xor_narg	==>
	{
		reg_t a;
		reg_t b;
		reg_t c;
		reg_t d;
		reg_t e;
		reg_t f;
		const_str_t a_cst_str;
		const_str_t b_cst_str;
		const_str_t c_cst_str;
		const_str_t d_cst_str;
		int n;
		const_str_t n_str;

		Comment0( xor_narg );
		if (type_of_tos() == T_cst)
		{
			n= pop_const(n_str);
			if (n == EM_WSIZE)
			{
				if (type_of_tos() == T_cst && const13(top_const()))
				{
					a= NULL;
					pop_const (a_cst_str);
				}
				else
					a= pop_reg();
				if (type_of_tos() == T_cst && const13(top_const()))
				{
					b= NULL;
					pop_const (b_cst_str);
				}
				else
					b= pop_reg();
				if (!a && !b)
				{
					a= alloc_reg();
					"mov	$a_cst_str, $a";	
				}
				c= alloc_reg();
				if (a)
					if (b)
						"xor	$a, $b, $c";
					else
						"xor	$a, $b_cst_str, $c";
				else
					"xor	$b, $a_cst_str, $c";
				free_reg(a);
				free_reg(b);
				push_reg(c);
			}
			else if (n == EM_DSIZE)
			{
				if (type_of_tos() == T_cst && const13(top_const()))
				{
					a= NULL;
					pop_const (a_cst_str);
				}
				else
					a= pop_reg();
				if (type_of_tos() == T_cst && const13(top_const()))
				{
					b= NULL;
					pop_const (b_cst_str);
				}
				else
					b= pop_reg();
				if (type_of_tos() == T_cst && const13(top_const()))
				{
					c= NULL;
					pop_const (c_cst_str);
				}
				else
					c= pop_reg();
				if (type_of_tos() == T_cst && const13(top_const()))
				{
					d= NULL;
					pop_const (d_cst_str);
				}
				else
					d= pop_reg();
				if (!b && !d)
				{
					b= alloc_reg();
					"mov	$b_cst_str, $b";	
				}
				e= alloc_reg();
				if (b)
					if (d)
						"xor	$b, $d, $e";
					else
						"xor	$b, $d_cst_str, $e";
				else
					"xor	$d, $b_cst_str, $e";
				free_reg(b);
				free_reg(d);
				push_reg(e);
				if (!a && !c)
				{
					a= alloc_reg();
					"mov	$a_cst_str, $a";	
				}
				e= alloc_reg();
				if (a)
					if (c)
						"xor	$a, $c, $e";
					else
						"xor	$a, $c_cst_str, $e";
				else
					"xor	$c, $a_cst_str, $e";
				free_reg(a);
				free_reg(c);
				push_reg(e);
			}
			else if (!(n % EM_WSIZE))
			{
				a= alloc_reg();
				b= alloc_reg();
				c= alloc_reg();
				d= alloc_reg();
				e= alloc_reg();
				f= alloc_reg();
				flush_cache();

				"set	$n_str, $a";
				"add	$reg_sp, $a, $b";
				"mov	$a, $c";
			"1:";
				"deccc	4, $c";
				"ld	[$reg_sp+$c], $d";
				"ld	[$b+$c], $e";
				"xor	$d, $e, $f";
				"bnz	1b";
				"st	$f, [$b+$c]";	/* delay slot */
				"add	$reg_sp, $a, $reg_sp";
				free_reg(a);
				free_reg(b);
				free_reg(c);
				free_reg(d);
				free_reg(e);
				free_reg(f);
			}
			else
				arg_error ("unimp xor", n);
		}
		else
		{
			a= pop_reg();
			b= alloc_reg();
			c= alloc_reg();
			d= alloc_reg();
			e= alloc_reg();
			f= alloc_reg();
			flush_cache();

			"add	$reg_sp, $a, $b";
			"mov	$a, $c";
		"1:";
			"deccc	4, $c";
			"ld	[$reg_sp+$c], $d";
			"ld	[$b+$c], $e";
			"xor	$d, $e, $f";
			"bnz	1b";
			"st	$f, [$b+$c]";	/* delay slot */
			"add	$reg_sp, $a, $reg_sp";
			free_reg(a);
			free_reg(b);
			free_reg(c);
			free_reg(d);
			free_reg(e);
			free_reg(f);
		}
	}.



C_com		==>
	Comment( com, $1);
	push_const($1);
	C_com_narg().

C_com_narg	==>
	{
		reg_t a;
		reg_t b;
		reg_t c;
		reg_t d;
		int n;
		int i;
		const_str_t i_str;

		Comment0( com_narg );
		if (type_of_tos() == T_cst && top_const() <= MAX_UNROLL)
		{
			n= pop_const(NULL);
			if (n == 4)
			{
				a= pop_reg();
				b= alloc_reg();
				"not	$a, $b";
				free_reg(a);
				push_reg(b);
			}
			else if (n == 8)
			{
				a= pop_reg();
				b= pop_reg();
				c= alloc_reg();
				d= alloc_reg();
				"not	$a, $c";
				"not	$b, $d";
				push_reg(d);
				push_reg(c);
				free_reg(b);
				free_reg(a);
			}
			else if (n>0 && !(n % 4))
			{
				flush_cache();
				a= alloc_reg();
				b= alloc_reg();
				for (i= 0; i< n; i += 4)
				{
					sprint(i_str, "%d", i);
					"ld	[$reg_sp+$i_str], $a";
					"not	$a, $b";
					"st	$b, [$reg_sp+$i_str]";
				}
			}
			else
				arg_error ("com", n);
		}
		else
		{
			a= pop_reg();
			b= alloc_reg();
			c= alloc_reg();
			flush_cache();
		"1:";
			"deccc	4, $a";
			"ld	[$reg_sp+$a], $b";
			"not	$a, $c";
			"bnz	1b";
			"st	$c, [$reg_sp+$a";
			free_reg(a);
			free_reg(b);
			free_reg(c);
		}
	}.

C_rol
	$1 == 4		==>
	{
		reg_t a;
		reg_t b;
		reg_t c;
		reg_t d;
		int n;
		const_str_t n_str;

		Comment( rol, $1);

		if (type_of_tos() == T_cst)
		{
			n= pop_const(NULL);
			if (n<0)
				arg_error("rol 4:", n);
			else
			{
				n= n % 32;
				if (n)
				{
					a= pop_reg();
					b= alloc_reg();
					c= alloc_reg();
					sprint(n_str, "%d", n);
					"sll	$a, $n_str, $b";
					sprint(n_str, "%d", 32-n);
					"srl	$a, $n_str, $c";
					"or	$b, $c, $a";
					push_reg(a);
					free_reg(b);
					free_reg(c);
				}
			}
		}
		else
		{
			a= pop_reg();
			b= pop_reg();
			c= alloc_reg();
			d= alloc_reg();
			"and	$a, 31, $c";
			"mov	32, $a";
			"sub	$a, $c, $d";
			"sll	$b, $c, $a";
			"srl	$b, $d, $c";
			"or	$a, $c, $b";
			free_reg(a);
			push_reg(b);
			free_reg(c);
			free_reg(d);
		}
	}.
	default		==>
				arg_error( "rol", $1).

narg4(rol)

C_ror
	$1 == 4		==>
	{
		reg_t a;
		reg_t b;
		reg_t c;
		reg_t d;
		int n;
		const_str_t n_str;

		Comment( ror, $1);

		if (type_of_tos() == T_cst)
		{
			n= pop_const(NULL);
			if (n<0)
				arg_error("ror 4:", n);
			else
			{
				n= n % 32;
				if (n)
				{
					a= pop_reg();
					b= alloc_reg();
					c= alloc_reg();
					sprint(n_str, "%d", n);
					"srl	$a, $n_str, $b";
					sprint(n_str, "%d", 32-n);
					"sll	$a, $n_str, $c";
					"or	$b, $c, $a";
					push_reg(a);
					free_reg(b);
					free_reg(c);
				}
			}
		}
		else
		{
			a= pop_reg();
			b= pop_reg();
			c= alloc_reg();
			d= alloc_reg();
			"and	$a, 31, $c";
			"mov	32, $a";
			"sub	$a, $c, $d";
			"srl	$b, $c, $a";
			"sll	$b, $d, $c";
			"or	$a, $c, $b";
			free_reg(a);
			push_reg(b);
			free_reg(c);
			free_reg(d);
		}
	}.
	default		==>
				arg_error( "ror", $1).

narg4(ror)

/******************************************************************************/
/*									      */
/*		Group 10 : Sets 	 				      */
/*									      */
/******************************************************************************/

C_inn		==>
	Comment( inn, $1);
	push_const($1);
	C_inn_narg().

C_inn_narg	==>
	{
		reg_t a;
		reg_t b;
		reg_t c;
		reg_t d;
		int i;
		int n;
		const_str_t i_str;
		const_str_t n_str;

		Comment0(inn_narg);
		if (type_of_tos() == T_cst && const13(top_const()))
		{
			n= pop_const(n_str);
			if (n == EM_WSIZE)
			{
				if (type_of_tos() == T_cst)
				{
					i= pop_const (i_str);
					if (i >= n*8)
						push_const(0);
					else
					{
						a= pop_reg();
						b= alloc_reg();
						c= alloc_reg();
						"srl	$a, $i_str, $b";
						"and	$b, 1, $c";
						free_reg(a);
						free_reg(b);
						push_reg(c);
					}
				}
				else
				{
					a= pop_reg();
					b= pop_reg();
					c= alloc_reg();
					"srl	$b, $a, $c";
					"and	$c, 1, $a";
					push_reg(a);
					free_reg(b);
					free_reg(c);
				}
			}
			else if (n == 2*EM_WSIZE)
			{
				if (type_of_tos() == T_cst)
				{
					i= pop_const (i_str);
					if (i >= n*8)
						push_const(0);
					else
					{
						if (i>= EM_WSIZE*8)
						{
							i -= EM_WSIZE*8;
							pop_nop(1);
							a= pop_reg();
						}
						else
						{
							a= pop_reg();
							pop_nop(1);
						}
						b= alloc_reg();
						c= alloc_reg();
						"srl	$a, $i_str, $b";
						"and	$b, 1, $c";
						free_reg(a);
						free_reg(b);
						push_reg(c);
					}
				}
				else
				{
					a= pop_reg();
					flush_cache();
					b= alloc_reg();
					c= alloc_reg();
					d= alloc_reg();
					flush_cache();
					"andn	$a, 31, $b";
					"and	$a, 31, $c";
					"srl	$b, 3, $d";
					"ld	[$reg_sp+$d], $b";
					"inc	$n_str, $reg_sp";
					"srl	$b, $c, $d";
					"and	$d, 1, $b";
					free_reg(a);
					push_reg(b);
					free_reg(c);
					free_reg(d);
				}
			}
			else if (n % EM_WSIZE)
				arg_error ("inn", n);
			else
			{
				a= pop_reg();
				flush_cache();
				b= alloc_reg();
				c= alloc_reg();
				d= alloc_reg();
				flush_cache();
				"andn	$a, 31, $b";
				"and	$a, 31, $c";
				"srl	$b, 3, $d";
				"ld	[$reg_sp+$d], $b";
				"inc	$n_str, $reg_sp";
				"srl	$b, $c, $d";
				"and	$d, 1, $b";
				free_reg(a);
				push_reg(b);
				free_reg(c);
				free_reg(d);
			}
		}
		else
			not_implemented ("inn_narg");
	}.

C_set	==>	Comment( set, $1);
		push_const($1);
		C_set_narg().

C_set_narg	==>
	{
		reg_t a;
		reg_t b;
		reg_t c;
		reg_t d;
		int n;
		const_str_t n_str;

		Comment0( set_narg );

		if (type_of_tos() == T_cst) {
			n = pop_const(n_str);
			if (n == EM_WSIZE) {
				b = alloc_reg();
				c = alloc_reg();
				a = pop_reg();
				"set	1, $c";
				"sll	$c, $a, $b";
				free_reg(a);
				free_reg(c);
				push_reg(b);
			} else {
				a= alloc_reg();
				b= pop_reg();
				c= alloc_reg();
				d= alloc_reg();
				flush_cache();
				sprint(n_str, "%d", n);
				"set	$n_str, $a";
				"sub	$reg_sp, $a, $reg_sp";
			"1:";
				"deccc	4, $a";
				"bnz	1b";
				"st	%g0, [$reg_sp+$a]";	/* HACK delay */
				"andn	$b, 31, $c";
				"and	$b, 31, $b";
				"srl	$c, 3, $c";
				"set	1, $a";
				"sll	$a, $b, $d";
				"st	$d, [$reg_sp+$c]";
				free_reg(a);
				free_reg(b);
				free_reg(c);
				free_reg(d);
			}
		} else {
			a= pop_reg();
			b= pop_reg();
			flush_cache();
			c= alloc_reg();
			d= alloc_reg();
			"sub	$reg_sp, $a, $reg_sp";
		"1:";
			"deccc	4, $a";
			"bnz	1b";
			"st	%g0, [$reg_sp+$a]";	/* HACK delay */
			"andn	$b, 31, $c";
			"and	$b, 31, $b";
			"srl	$c, 3, $c";
			"set	1, $a";
			"sll	$a, $b, $d";
			"st	$d, [$reg_sp+$c]";
			free_reg(a);
			free_reg(b);
			free_reg(c);
			free_reg(d);
		}
	}.


/******************************************************************************/
/*									      */
/*		Group 11 : Array 	 				      */
/*									      */
/******************************************************************************/

C_lar
	($1 == 4) ==>
			Comment(lar, $1);
			force_alloc_output();
			pop_reg_as(reg_o0);
			pop_reg_as(reg_o1);
			pop_reg_as(reg_o2);
			flush_cache();
			"call lar";
			"nop";
			free_output().
	default ==>
			arg_error ("arg error lar", $1).

narg4(lar)

C_sar
	($1 == 4) ==>
			Comment( sar , $1 );
			force_alloc_output();
			pop_reg_as(reg_o0);
			pop_reg_as(reg_o1);
			pop_reg_as(reg_o2);
			flush_cache();
			"call	sar";
			"nop"
			free_output().
	default ==>
			arg_error ("arg error sar", $1).

narg4(sar)

C_aar
	($1 == 4) ==>
			Comment(aar, $1);
			force_alloc_output();
			pop_reg_as(reg_o0);
			pop_reg_as(reg_o1);
			pop_reg_as(reg_o2);
			flush_cache();
			"call	aar";
			"nop";
			soft_alloc_reg(reg_o0);
			free_output();
			push_reg(reg_o0).
	default ==>
			arg_error ("arg error aar", $1).

narg4(aar)

/******************************************************************************/
/*									      */
/*		Group 12 : Compare 	 				      */
/*									      */
/******************************************************************************/

C_cmi
	$1 == 4		==>
	{
		reg_t a;
		reg_t b;
		reg_t c;
		const_str_t d;

		Comment( cmi, $1 );

		if (type_of_tos() == T_cst && const13(top_const()))
		{
			pop_const(d);
			a= pop_reg();
			b= alloc_reg();
			"cmp	$a, $d";
			"be,a	1f";
			"mov	0, $b"; /* delay slot */
			"bg,a	1f";
			"mov	1, $b"; /* delay slot */
			"mov	-1, $b";
		"1:";
			free_reg(a);
			push_reg(b);
		}
		else
		{
			a= pop_reg();
			b= NULL;
			c= alloc_reg();
			if (type_of_tos() == T_cst)
			{
				pop_const(d);
				"cmp	$a, $d";
			}
			else
			{
				b= pop_reg();
				"cmp	$a, $b";
			}
			"be,a	1f";
			"mov	0, $c"; /* delay slot */
			"bg,a	1f";
			"mov	-1, $c"; /* delay slot */
			"mov	1, $c";
		"1:";
			free_reg(a);
			if (b)
				free_reg(b);
			push_reg(c);
		}
	}.
	default		==>
		arg_error ("unimp cmi", $1).

narg4(cmi)

C_cmu	==>
		Comment( cmu, $1 );
		push_const($1);
		C_cmu_narg().

C_cmu_narg	==>
	{
		int n;
		reg_t a;
		reg_t b;
		reg_t c;
		const_str_t d;

		Comment0( cmu_narg );
		if (type_of_tos() == T_cst)
		{
			n= pop_const(NULL);
			if (n != EM_WSIZE)
				arg_error ("unimp cmu", n);
			else
			{
				if (type_of_tos() == T_cst &&
					const13(top_const()))
				{
					pop_const(d);
					a= pop_reg();
					b= alloc_reg();
					"cmp	$a, $d";
					"be,a	1f";
					"mov	0, $b"; /* delay slot */
					"bgu,a	1f";
					"mov	1, $b"; /* delay slot */
					"mov	-1, $b";
				"1:";
					free_reg(a);
					push_reg(b);
				}
				else
				{
					a= pop_reg();
					b= NULL;
					c= alloc_reg();
					if (type_of_tos() == T_cst &&
						const13(top_const()))
					{
						pop_const(d);
						"cmp	$a, $d";
					}
					else
					{
						b= pop_reg();
						"cmp	$a, $b";
					}
					"be,a	1f";
					"mov	0, $c"; /* delay slot */
					"bgu,a	1f";
					"mov	-1, $c"; /* delay slot */
					"mov	1, $c";
				"1:";
					free_reg(a);
					if (b)
						free_reg(b);
					push_reg(c);
				}
			}
		}
		else
			not_implemented ("cmu_narg");
	}.

C_cms	==>
		Comment( cms, $1 );
		push_const($1);
		C_cms_narg().

C_cms_narg	==>
	{
		int n;
		reg_t a;
		reg_t b;
		reg_t c;
		reg_t d;
		reg_t e;
		const_str_t b_str;
		const_str_t n_str;

		Comment0( cms_narg );
		if (type_of_tos() == T_cst && top_const() <= EM_WSIZE)
		{
			n= pop_const(n_str);
			if (n == EM_WSIZE)
			{
				b= NULL;
				c= alloc_reg();
				if (type_of_tos() == T_cst &&
				    const13(top_const()))
				{
					pop_const(b_str);
					a= pop_reg();
					"cmp	$a, $b_str";
				}
				else
				{
					a= pop_reg();
					b= pop_reg();
					"cmp	$a, $b";
				}
				"be,a	1f";
				"mov	0, $c";
				"mov	1, $c";
			"1:";
				free_reg(a);
				if (b)
					free_reg(b);
				push_reg(c);
			}
			else if (n % EM_WSIZE)
				arg_error ("unimp cms", n);
		}
		else
		{
			a= pop_reg();
			flush_cache();
			b= alloc_reg();
			c= alloc_reg();
			d= alloc_reg();

			"add	$reg_sp, $a, $b";
			"dec	4, $b";
		"1:";
			"ld	[$b], $c";
			"ld	[$b+$a], $d";
			"cmp	$d, $c";
			"bne,a	2f";
			"mov	1, $b";	/* delay slot */
			"cmp	$b, $reg_sp";
			"bg	1b";
			"dec	4, $b";	/* delay slot */
			"mov	0, $b";
		"2:";
			"add	$reg_sp, $a, $reg_sp";
			"add	$reg_sp, $a, $reg_sp";

			free_reg(a);
			push_reg(b);
			free_reg(c);
			free_reg(d);
		}
	}.

C_cmp		==>
			Comment0( cmp );
			C_cmu( (arith)4).

C_tlt	==>
		Comment0( tlt );
		{
			reg_t a;
			reg_t b;

			a = pop_reg();
			b= alloc_reg();
			"	tst	$a";
			"	bl,a	1f";
			"	mov	1, $b";		/* delay slot */
			"	set	0, $b";
			"1:";
			free_reg(a);
			push_reg(b);
		}.

C_tle	==>
		Comment0( tle );
		{
			reg_t a;
			reg_t b;

			a = pop_reg();
			b= alloc_reg();
			"tst	$a";
			"ble,a	1f";
			"mov	1, $b";		/* delay slot */
			"set	0, $b";
		"1:";
			free_reg(a);
			push_reg(b);
		}.

C_tge	==>
		Comment0( tge );
		{
			reg_t a;
			reg_t b;

			a = pop_reg();
			b = alloc_reg();
			"	tst	$a";
			"	bge,a	1f";
			"	mov	1, $b";		/* delay slot */
			"	set	0, $b";
			"1:";
			free_reg(a);
			push_reg(b);
		}.

C_tgt	==>
		Comment0( tgt );
		{
			reg_t a;
			reg_t b;

			a = pop_reg();
			b = alloc_reg();
			"	tst	$a";
			"	bg,a	1f";
			"	mov	1, $b";		/* delay slot */
			"	set	0, $b";
			"1:";
			free_reg(a);
			push_reg(b);
		}.

C_tne	==>
		Comment0( tne );
		{
			reg_t a;
			reg_t b;

			a = pop_reg();
			b = alloc_reg();
			"	tst	$a";
			"	bne,a	1f";
			"	mov	1, $b";		/* delay slot */
			"	set	0, $b";		/* sup optimal */
			"1:";
			free_reg(a);
			push_reg(b);
		}.

C_teq	==>
		Comment0( teq );
		{
			reg_t a;
			reg_t b;

			a = pop_reg();
			b = alloc_reg();
			"	tst	$a";
			"	be,a	1f";
			"	mov	1, $b";		/* delay slot */
			"	set	0, $b";
			"1:";
			free_reg(a);
			push_reg(b);
		}.
C_cmf		==>
	Comment( cmf, $1);
	push_const($1);
	C_cmf_narg().

C_cmf_narg	==>
	{
		reg_t a;
		reg_t b;
		reg_t c;
		int n;

		Comment0( cmf_narg);
		if (type_of_tos() == T_cst)
		{
			n= pop_const(NULL);
			if (n == EM_FSIZE)
			{
				a= pop_float();
				b= pop_float();
				c= alloc_reg();
				"fcmpes	$b, $a";
				"nop";
				"fbe,a	1f";
				"mov	0, $c";
				"fbl,a	1f";
				"mov	-1, $c";
				"mov	1, $c";
			"1:";
				free_reg(a);
				free_reg(b);
				push_reg(c);
			}
			else if (n == EM_DSIZE)
			{
				a= pop_double(NULL);
				b= pop_double(NULL);
				c= alloc_reg();
				"fcmped	$b, $a";
				"nop";
				"fbe,a	1f";
				"mov	0, $c";
				"fbl,a	1f";
				"mov	-1, $c";
				"mov	1, $c";
			"1:";
				free_double_reg(a);
				free_double_reg(b);
				push_reg(c);
			}
			else
				arg_error ("cmf", n);
		}
		else
			not_implemented ("cmf_narg");
	}.

/******************************************************************************/
/*									      */
/*		Group 13 : Branch 	 				      */
/*									      */
/******************************************************************************/

C_bra		==>
			Comment( bra , $1 );
			{
				char *lbl;

				flush_cache();
				lbl = $1;
				"b	$lbl";
				"nop";		/* delay slot */
			}.

C_bge	==>
		Comment( bge , $1 );
		{
			char *lbl = $1;
			reg_t a;
			reg_t b;
			const_str_t n_str;

			a= NULL;
			if (type_of_tos() == T_cst &&
				const13(top_const()))
				pop_const(n_str);
			else
				a = pop_reg();
			b = pop_reg();
			flush_cache();
			if (a)
				"cmp	$b, $a";
			else
				"cmp	$b, $n_str";
			"bge	$lbl";
			"nop"		/* delay slot */
			free_reg(a);
			free_reg(b);
		}.

C_bne	==>
		Comment( bne , $1 );
		{
			char *lbl = $1;
			reg_t a;
			reg_t b;
			const_str_t n_str;

			a= NULL;
			if (type_of_tos() == T_cst &&
				const13(top_const()))
				pop_const(n_str);
			else
				a = pop_reg();
			b = pop_reg();
			flush_cache();
			if (a)
				"cmp	$b, $a";
			else
				"cmp	$b, $n_str";
			"bne	$lbl";
			"nop"		/* delay slot */
			free_reg(a);
			free_reg(b);
		}.

C_beq	==>
		Comment( beq , $1 );
		{
			char *lbl = $1;
			reg_t a;
			reg_t b;
			const_str_t n_str;

			a= NULL;
			if (type_of_tos() == T_cst &&
				const13(top_const()))
				pop_const(n_str);
			else
				a = pop_reg();
			b = pop_reg();
			flush_cache();
			if (a)
				"cmp	$b, $a";
			else
				"cmp	$b, $n_str";
			"beq	$lbl";
			"nop"		/* delay slot */
			free_reg(a);
			free_reg(b);
		}.

C_ble	==>
		Comment( ble , $1 );
		{
			char *lbl = $1;
			reg_t a;
			reg_t b;
			const_str_t n_str;

			a= NULL;
			if (type_of_tos() == T_cst &&
				const13(top_const()))
				pop_const(n_str);
			else
				a = pop_reg();
			b = pop_reg();
			flush_cache();
			if (a)
				"cmp	$b, $a";
			else
				"cmp	$b, $n_str";
			"ble	$lbl";
			"nop"		/* delay slot */
			free_reg(a);
			free_reg(b);
		}.

C_blt	==>
		Comment( blt , $1 );
		{
			char *lbl = $1;
			reg_t a;
			reg_t b;
			const_str_t n_str;

			a= NULL;
			if (type_of_tos() == T_cst &&
				const13(top_const()))
				pop_const(n_str);
			else
				a = pop_reg();
			b = pop_reg();
			flush_cache();
			if (a)
				"cmp	$b, $a";
			else
				"cmp	$b, $n_str";
			"bl	$lbl";
			"nop"		/* delay slot */
			free_reg(a);
			free_reg(b);
		}.

C_bgt	==>
		Comment( bgt , $1 );
		{
			char *lbl = $1;
			reg_t a;
			reg_t b;
			const_str_t n_str;

			a= NULL;
			if (type_of_tos() == T_cst &&
				const13(top_const()))
				pop_const(n_str);
			else
				a = pop_reg();
			b = pop_reg();
			flush_cache();
			if (a)
				"cmp	$b, $a";
			else
				"cmp	$b, $n_str";
			"bg	$lbl";
			"nop"		/* delay slot */
			free_reg(a);
			free_reg(b);
		}.

C_zlt	==>
		Comment( zlt , $1 );
		{
			char *lbl = $1;
			reg_t a;

			a = pop_reg();
			flush_cache();
			"tst	$a";
			"bl	$lbl";
			"nop";		/* delay slot */
			free_reg(a);
		}
		.

C_zle	==>
		Comment( zle , $1 );
		{
			char *lbl = $1;
			reg_t a;

			a = pop_reg();
			flush_cache();
			"tst	$a";
			"ble	$lbl";
			"nop";		/* delay slot */
			free_reg(a);
		}
		.

C_zeq	==>
		Comment( zeq , $1 );
		{
			char *lbl = $1;
			reg_t a;

			a = pop_reg();
			flush_cache();
			"tst	$a";
			"be	$lbl";
			"nop";		/* delay slot */
			free_reg(a);
		}
		.

C_zne	==>
		Comment( zne , $1 );
		{
			char *lbl = $1;
			reg_t a;

			a = pop_reg();
			flush_cache();
			"tst	$a";
			"bne	$lbl";
			"nop";		/* delay slot */
			free_reg(a);
		}
		.

C_zge	==>
		Comment( zge , $1 );
		{
			char *lbl = $1;
			reg_t a;

			a = pop_reg();
			flush_cache();
			"tst	$a";
			"bge	$lbl";
			"nop";		/* delay slot */
			free_reg(a);
		}
		.

C_zgt	==>
		Comment( zgt , $1 );
		{
			char *lbl = $1;
			reg_t a;

			a = pop_reg();
			flush_cache();
			"tst	$a";
			"bg	$lbl";
			"nop";		/* delay slot */
			free_reg(a);
		}
		.
/******************************************************************************/
/*                                                                            */
/*			Group 14 : Procedure call instructions		      */
/*							 		      */
/******************************************************************************/

C_cai		==>
			Comment0( cai );
			{
				reg_t a;

				a= pop_reg();
				flush_cache();
				"call	$a";
				"nop";		/* delay slot */
				free_reg(a);
			}.

C_cal		==>
			Comment( cal , $1 );
			{
				char *lbl = $1;
				flush_cache();
				"call	$lbl";
				"nop";		/* delay slot */
			}.

C_lfr
	$1 == 4		==>
	{
		Comment( lfr , $1 );
		forced_alloc_reg(RETL_LD);
		push_reg(RETL_LD);
	}.
	$1 == 8		==>
	{
		Comment( lfr , $1 );
		forced_alloc_reg(RETL_LD);
		forced_alloc_reg(RETH_LD);
		push_reg(RETH_LD);
		push_reg(RETL_LD);
	}.
	default		==>
		arg_error( "lfr", $1).

C_ret
	$1 == 0		==>
	{
		Comment( ret , $1 );
		load_float_regs();
		if (debug)
			free_all_reg_vars();
		"restore";
		"retl";
		"add	%sp, $reg_gap, %sp";
		if (debug)
			alloc_all_reg_vars();
	}.
	$1 == 4		==>
	{
		Comment( ret , $1 );
		soft_alloc_reg(RETL_ST);
		pop_reg_as(RETL_ST);
		free_reg(RETL_ST);
		load_float_regs();
		if (debug)
			free_all_reg_vars();
		"restore";
		"retl";
		"add	%sp, $reg_gap, %sp";
		if (debug)
			alloc_all_reg_vars();
	}.
	$1 == 8		==>
	{
		Comment( ret , $1 );
		soft_alloc_reg(RETL_ST);
		soft_alloc_reg(RETH_ST);
		pop_reg_as(RETL_ST);
		pop_reg_as(RETH_ST);
		free_reg(RETL_ST);
		free_reg(RETH_ST);
		load_float_regs();
		if (debug)
			free_all_reg_vars();
		"restore";
		"retl";
		"add	%sp, $reg_gap, %sp";
		if (debug)
			alloc_all_reg_vars();
	}.
	default		==>
				arg_error( "ret", $1).

/******************************************************************************/
/*                                                                            */
/*			Group 15 : Miscellaneous instructions		      */
/*							 		      */
/******************************************************************************/

C_asp		==>
	Comment( asp , $1 );
	push_const($1);
	C_ass(EM_WSIZE).


C_ass
	$1 == 4		==>
	{
		int n;
		const_str_t n_str;
		reg_t a;

		if (type_of_tos() == T_cst)
		{
			n= pop_const(n_str);
			if (n % EM_WSIZE)
				arg_error ("asp", n);
			else
				if (n>=0)
					pop_nop (n/4);
				else
				{
					flush_cache();
					if (const13(n))
						"inc	$n_str, $reg_sp";
					else
					{
						a= alloc_reg();
						"set	$n_str, $a"
						"add	$reg_sp, $a, $reg_sp";
						free_reg(a);
					}
				}
		}
		else
		{
			a= pop_reg();
			flush_cache();
			"add	$reg_sp, $a, $reg_sp";
			free_reg(a);
		}
	}.
	default		==>
				arg_error( "ass", $1).


narg4(ass)

C_blm		==>
	Comment( blm, $1);
	push_const($1);
	C_bls (EM_WSIZE).

C_bls
	$1 == 4	==>
	{
		reg_t a;
		reg_t b;
		reg_t c;
		reg_t d;
		reg_t ao_reg;
		reg_t bo_reg;
		int n;
		int i;
		const_str_t n_str;
		const_str_t ac_str;
		const_str_t bc_str;
		
		Comment( bls , $1 );
		if (type_of_tos() == T_cst)
		{
			n= pop_const(n_str);
			if (n % EM_WSIZE)
				arg_error ("blm", n);
			else if (n <= MAX_UNROLL)
			{
				c= alloc_reg();
				for (i=0; i<n; i += EM_WSIZE)
				{
					if (type_of_tos() & T_reg2) /* dest */
						a= pop_reg_reg (&ao_reg);
					else
					{
						ao_reg= NULL;
						a= pop_reg_c13(ac_str);
					}
					if (type_of_tos() & T_reg2) /* src */
						b= pop_reg_reg (&bo_reg);
					else
					{
						bo_reg= NULL;
						b= pop_reg_c13(bc_str);
					}
					if (bo_reg)
						"ld	[$b+$bo_reg], $c";
					else
						"ld	[$b+$bc_str], $c";
					if (ao_reg)
						"st	$c, [$a+$ao_reg]";
					else
						"st	$c, [$a+$ac_str]";
					if (bo_reg)
					{
						push_reg(b);
						inc_tos_reg(bo_reg);
					}
					else if (bc_str[0] == '-' ||
						isdigit(bc_str[0]))
					{
						push_reg(b);
						inc_tos(atoi(bc_str));
					}
					else
					{
						"add	$b, $bc_str, $c";
						push_reg(c);
						free_reg(b);
						c= alloc_reg();
					}
					inc_tos(4);
					if (ao_reg)
					{
						push_reg(a);
						inc_tos_reg(ao_reg);
					}
					else if (ac_str[0] == '-' ||
						isdigit(ac_str[0]))
					{
						push_reg(a);
						inc_tos(atoi(ac_str));
					}
					else
					{
						"add	$a, $ac_str, $c";
						push_reg(c);
						free_reg(a);
						c= alloc_reg();
					}
					inc_tos(4);
				}
				pop_nop(2);
				free_reg(c);
			}
			else
			{
				a= pop_reg();	/* dest */
				b= pop_reg();	/* src */
				c= alloc_reg();
				d= alloc_reg();
				"set	$n_str, $c";
			"1:";
				"deccc	4, $c";
				"ld	[$b+$c], $d";
				"bnz	1b";
				"st	$d, [$a+$c]";
				free_reg(a);
				free_reg(b);
				free_reg(c);
				free_reg(d);
			}
		}
		else
		{

			c= pop_reg();	/* size */
			a= pop_reg();	/* dest */
			b= pop_reg();	/* src */
			d= alloc_reg();
		"1:";
			"deccc	4, $c";
			"ld	[$b+$c], $d";
			"bnz	1b";
			"st	$d, [$a+$c]";
			free_reg(a);
			free_reg(b);
			free_reg(c);
			free_reg(d);
		}
	}.

	default		==>
				arg_error( "bls", $1).

narg4(bls)

C_csa
	$1 == 4	==>
		Comment( csa , $1 );
		{
			force_alloc_output();
			pop_reg_as(reg_o0);
			pop_reg_as(reg_o1);
			flush_cache();
			free_output();
			"set	csa, $reg_tmp";
			"jmp	$reg_tmp";
			"nop";
		}.
	default	==>
		arg_error( "csa", $1).

narg4(csa)

C_csb
	$1 == 4		==>
			Comment( csb , $1 );
			{
				force_alloc_output();
				pop_reg_as(reg_o0);
				pop_reg_as(reg_o1);
				flush_cache();
				free_output();
				"set	csb, $reg_tmp";
				"jmp	$reg_tmp";
				"nop";
			}.
	default		==>
				arg_error( "csb", $1).

narg4(csb)

C_dch		==>
	{
		reg_t a;
		reg_t b;
		reg_t c;

		Comment0( dch );
		a= pop_reg();		/* some LB */
		b= alloc_reg();
		c= alloc_reg();
		"ta	3";		/* flush register windows */
		"add	$a, 7, $b";
		"andn	$b, 7, $c";	/* and it's %fp */
		"ld	[$c+4], $b";	/* the previous LB */
		free_reg(a);
		push_reg(b);
		free_reg(c);
	}.

C_dup	==>
		Comment( dup, $1);
		push_const($1);
		C_dus(EM_WSIZE).

C_dus
	$1 == 4 ==>
	{
		int n;
		int i;
		const_str_t n_str;
		const_str_t i_str;
		reg_t a;
		reg_t b;
		reg_t c;

		Comment( dus, $1);

		if (type_of_tos() == T_cst && top_const() <= MAX_UNROLL)
		{
			n= pop_const(n_str);
			if (n == 4 || n == 8 || n<=32)
				dup_tos(n/4);
			else if (n<0 || n % 4)
				arg_error ("dup", n);
			else
			{
				flush_cache();
				a= alloc_reg();
				"sub	$reg_sp, $n_str, $reg_sp";
				for (i=0; i<n; i += 4)
				{
					sprint(i_str, "%d", i);
					"ld	[$reg_sp+$i_str+$n_str], $a";
					"st	$a, [$reg_sp+$i_str]";
				}
				free_reg(a);
			}
		}
		else
		{
			a= pop_reg();
			flush_cache();
			b= alloc_reg();
			c= alloc_reg();
			"mov	$a, $b";
		"1:";
			"dec	STACK_CLICK, $reg_sp";
			"ld	[$reg_sp+ $a], $c";
			"deccc	4, $b";
			"bne	1b";
			"st	$c, [$reg_sp+ $a]"; /* delay slot */
			free_reg(a);
			free_reg(b);
			free_reg(c);
		}
	}.
	default		==>
				arg_error( "dus", $1).

narg4(dus)

C_exg		==>
	Comment( exg, $1 );
	push_const($1);
	C_exg_narg().

C_exg_narg	==>
	{
		reg_t a;
		reg_t b;
		reg_t c;
		reg_t d;
		int n;
		int i;
		const_str_t i_str;
		const_str_t in_str;

		Comment0( exg_narg );
		if (type_of_tos() == T_cst && top_const() <= MAX_UNROLL)
		{
			n= pop_const(NULL);
			if (n==4)
			{
					a= pop_reg();
					b= pop_reg();
					push_reg(a);
					push_reg(b);
			}
			else if (n==8)
			{
				a= pop_reg();
				b= pop_reg();
				c= pop_reg();
				d= pop_reg();
				push_reg(b);
				push_reg(a);
				push_reg(d);
				push_reg(c);
			}
			else if (n>0 && !(n % 4))
			{
				a= alloc_reg();
				b= alloc_reg();
				flush_cache();
				for (i=0; i<n; i += 4)
				{
					sprint(i_str, "%d", i);
					sprint(in_str, "%d", i+n);
					"ld	[$reg_sp+$i_str], $a";
					"ld	[$reg_sp+$in_str], $b";
					"st	$b, [$reg_sp+$i_str]";
					"st	$a, [$reg_sp+$in_str]";
				}
				free_reg(a);
				free_reg(b);
			}
			else
				arg_error ("exg", n);
		}
		else
		{
			a= pop_reg();
			flush_cache();
			b= alloc_reg();
			c= alloc_reg();
			d= alloc_reg();
			"add	$reg_sp, $a, $b";
		"1:";
			"dec	4, $b";
			"cmp	$reg_sp, $b";
			"ld	[$b], $c";
			"ld	[$b+$a], $d";
			"st	$d, [$b]";
			"bne	1b";
			"st	$c, [$b+$a]";	/* delay slot */
			free_reg(a);
			free_reg(b);
			free_reg(c);
			free_reg(d);
		}
	}.

C_fil..		==>
			Comment2( fil , $1 , $2);
#ifdef FAST_LIN_LNI_FIL
			{
				char *lbl = $1;
				int n = $2;

				"set	$lbl+$n, $reg_fil"
			}.
#else
			push_ext($1);
			inc_tos($2);
			push_ext("filn");
			push_const(4);
			C_sts(EM_WSIZE).
#endif


C_gto..		==>
	{
		char *ext;
		reg_t a;
		reg_t b;
		reg_t c;
		reg_t d;

		Comment2( gto , $1 , $2 );

		flush_cache();
		a= reg_g1;
		b= reg_g2;
		c= reg_g3;
		d= reg_g5;
		forced_alloc_reg(a);
		forced_alloc_reg(b);
		forced_alloc_reg(c);
		forced_alloc_reg(d);
		ext= $1;
		push_ext(ext);
		inc_tos($2);
		pop_reg_as(a);
		"ld	[$a+8], $b";
		"mov	$reg_o0, $c";
		"mov	$reg_o1, $d";
	"1:";
		"cmp	$b, $reg_lb";
		"bne,a	1b";
		"restore";
		"ld	[$a+4], $reg_sp";
		"ld	[$a], $b";
		"mov	$c, $reg_o0";
		"jmp	$b";
		"mov	$d, $reg_o1";	/* delay slot */
		free_reg(a);
		free_reg(b);
		free_reg(c);
		free_reg(d);
	}.

C_lim		==>
			Comment0( lim  );
			push_ext("trpim");
			C_loi(4).

C_lin		==>
			Comment( lin , $1 );
#ifdef FAST_LIN_LNI_FIL
			{
				const_str_t n_str;
				sprint(n_str, "%d", $1);
				"set	$n_str, $reg_fil";
			}.
#else
			push_const($1);
			push_ext("lino");
			push_const(4);
			C_sts(EM_WSIZE).
#endif

C_lni		==>
#ifdef FAST_LIN_LNI_FIL
			Comment0( lni );
		 	"inc	$reg_fil".
#else
			{
				reg_t a;
				reg_t b;

				Comment0( lni );
				a = alloc_reg();
				b = alloc_reg();
				"sethi	%hi(lino), $a";
				"ld	[$a+%lo(lino)], $b";
				"inc	$b";
				"st	$b, [$a+%lo(lino)]"
				free_reg(a);
				free_reg(b);
			}.
#endif


C_lor
	$1 == 0		==>
				Comment( lor , $1 );
				soft_alloc_reg(reg_lb);
				push_reg(reg_lb).
	$1 == 1		==>
	{
		reg_t a;

		Comment( lor , $1 );
		a= alloc_reg();
		flush_cache();
		"mov	$reg_sp, $a";
		push_reg(a);
	}.
	$1 == 2		==>
				Comment( lor , $1 );
				{
					reg_t a;
					reg_t b;

					a= alloc_reg();
					b= alloc_reg();
					"set	reghp, $a";
					"ld	[$a], $b";
					push_reg(b);
					free_reg(a);
				}.

	default		==>
				arg_error( "lor", $1).


C_lpb		==>
			Comment0( lpb );
			C_adp( (arith)EM_BSIZE).


C_mon		==>
			Comment0( mon );
			force_alloc_output();
			pop_reg_as(reg_o0);
			"call mon";
			"nop";
			free_output().

C_nop	==>
			Comment0( nop );
			flush_cache();
			.


C_rck
	$1 == 4		==>
	{
		reg_t a;
		reg_t b;
		reg_t c;

		Comment( rck , $1 );
		force_alloc_output();
		a= pop_reg();
		b= pop_reg();
		soft_alloc_reg(b);
		push_reg(b);
		c= alloc_reg();
		"ld	[$a], $c";
		"cmp	$b, $c";
		"bl	1f";
		"ld	[$a+4], $c";
		"cmp	$b, $c";
		"ble	2f";
		"nop";
	"1:";
		"set	ERANGE, $reg_o0";
		"call	trp";
		"nop";
	"2:";
		free_reg(a);
		free_reg(b);
		free_reg(c);
		free_output();
	}.
	default		==>
		arg_error( "rck", $1).

narg4(rck)

C_rtt		==>
			Comment0( rtt );
			C_ret( (arith)0).


C_sig		==>
			Comment0( sig );
			{
				reg_t a;
				reg_t b;
				reg_t c;

				a= pop_reg();
				b= alloc_reg();
				c= alloc_reg();
				"set	trppc, $b";
				"ld	[$b], $c";
				"st	$a, [$b]";
				free_reg(a);
				free_reg(b);
				push_reg(c);
			}.


C_sim		==>
			Comment0( sim );
			{
				reg_t a;
				reg_t b;

				a= pop_reg();
				b= alloc_reg();
				"set	trpim, $b";
				"st	$a, [$b]";
				free_reg(a);
				free_reg(b);
			}.


C_str
	$1 == 0		==>
				Comment( str , $1 );
				flush_cache();
				"ld	[$reg_sp], $reg_lb";
				"add	$reg_lb, 4, %fp";
				"and	%fp, -8, %fp";
				"inc	STACK_CLICK, $reg_sp"
				.
	$1 == 1		==>
	{
		Comment( str , $1 );
		flush_cache();
		"ld [$reg_sp], $reg_sp";
	}.
	$1 == 2		==>
	{
		Comment( str , $1 );
		force_alloc_output();
		pop_reg_as(reg_o0);
		"call	strhp";
		"nop";
		free_output();
	}.
	default		==>
				arg_error( "str", $1).


C_trp		==>
			Comment0( trp );
			force_alloc_output();
			pop_reg_as(reg_o0);
			flush_cache();
			"call	trp";
			"nop";
			free_output().

/*****************************************************************************/

..icon
	$2 == 1  	==>
				Comment( ..icon , $1 );
				gen1( (ONE_BYTE) atoi( $1)).
	$2 == 2  	==>
				Comment( ..icon , $1 );
				gen2( (TWO_BYTES) atoi( $1)).
	$2 == 4  	==>
				Comment( ..icon , $1 );
				gen4( (FOUR_BYTES) atol( $1)).
	default     	==>
				arg_error( "icon", $2).

..ucon
	$2 == 1  	==>
				Comment( ..ucon , $1 );
				gen1( (ONE_BYTE) atoi( $1)).
	$2 == 2  	==>
				Comment( ..ucon , $1 );
				gen2( (TWO_BYTES) atoi( $1)).
	$2 == 4  	==>
				Comment( ..ucon , $1 );
				gen4( (FOUR_BYTES) atol( $1)).
	default     	==>
				arg_error( "icon", $2).

..fcon			==>
				Comment( ..fcon , $1 );
				con_float($1, $2).

/*****************************************************************************/

C_prolog		==>
	Comment0( prolog );
	init_cache();
	"sub	$reg_sp, (EM_BSIZE-4), %g1";
	"and	%g1, -8, %sp";
	"mov	$reg_sp, %g1";
	"save	%sp, $reg_gap, %sp";
	"st	%g0, [%sp+BP_OFFSET]";
	"sub	%g1, EM_BSIZE, $reg_lb";
	init_reg_man();
	forced_alloc_reg(reg_sp);
	forced_alloc_reg(reg_lb);
	forced_alloc_reg(reg_gap);
#ifdef FAST_LIN_LNI_FIL
	reg_lin = alloc_reg();
	reg_fil = alloc_reg();
#endif
	.
C_jump	 		==>
	{
		char *l;

		Comment( jump , $1 );
		l= $1;
		"b	$l";
		"nop";	/* delay slot */
	}.

C_locals		==>
	{
		Comment( locals , $1 );

		soft_alloc_reg(reg_sp);
		push_reg(reg_lb);
		inc_tos(-($1));
		pop_reg_as(reg_sp);
	}.
