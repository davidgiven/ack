#if WORD_SIZE==2
#define LLP ldl
#define LEP lde
#define LFP ldf		/* load offsetted pointer */
#define SLP sdl
#define SEP sde
#define SFP sdf		/* store offsetted pointer */

#define ABS_off_int	ABS_off2
#define ABS_indoff_int	ABS_indoff2
#define ABSIND_off_int	ABSIND_off2
#define INDOFF_off_int	INDOFF_off2
#define OFF_off_int	OFF_off2
#define OFF_indoff_int	OFF_indoff2
#define abs_index_int	abs_index2
#define absolute_int	absolute2
#define any_int		any2
#define conreg_int	conreg2
#define data_int	data2
#define datalt_int	datalt2
#define dreg_int	dreg2
#define imm_cmp_int	imm_cmp2
#define immediate_int	immediate2
#define indirect_int	indirect2
#define index_off_int	index_off2
#define offsetted_int	offsetted2
#define post_inc_int	post_inc2
#define pre_dec_int	pre_dec2
#define store_int	any2
#define test_set_int	test_set2

#define add_i	add_w
#define ADD_I	"add.w"
#define and_i	and_w
#define AND_I	"and.w"
#define asl_i	asl_w
#define ASL_I	"asl.w"
#define asr_i	asr_w
#define ASR_I	"asr.w"
#define clr_i	clr_w
#define CLR_I	"clr.w"
#define cmp_i	cmp_w
#define cmp2_i	cmp2_w
#define DEC	"sub.w #1,"
#define DIVS_I	"divs.w"
#define DIVU_I	"divu.w"
#define eor_i	eor_w
#define EOR_I	"eor.w"
#define INC	"add.w #1,"
#define lsr_i	lsr_w
#define LSR_I	"lsr.w"
#define move_i	move_w
#define MOVE_I	"move.w"
#define muls_i	muls_w
#define MULS_I	"muls.w"
#define mulu_i	mulu_w
#define MULU_I	"mulu.w"
#define neg_i	neg_w
#define NEG_I	"neg.w"
#define not_i	not_w
#define NOT_I	"not.w"
#define or_i	or_w
#define OR_I	"or.w"
#define rol_i	rol_w
#define ROL_I	"rol.w"
#define ror_i	ror_w
#define ROR_I	"ror.w"
#define sub_i	sub_w
#define SUB_I	"sub.w"
#define tst_i	tst_w

#else

#define LLP lol
#define LEP loe
#define LFP lof		/* load offsetted pointer */
#define SLP stl
#define SEP ste
#define SFP stf		/* store offsetted pointer */

#define ABS_off_int	ABS_off4
#define ABS_indoff_int	ABS_indoff4
#define ABSIND_off_int	ABSIND_off4
#define INDOFF_off_int	INDOFF_off4
#define OFF_off_int	OFF_off4
#define OFF_indoff_int	OFF_indoff4
#define abs_index_int	abs_index4
#define absolute_int	absolute4
#define any_int		any4
#define conreg_int	conreg4
#define data_int	data4
#define datalt_int	datalt4
#define dreg_int	dreg4
#define imm_cmp_int	imm_cmp4
#define immediate_int	immediate4
#define indirect_int	indirect4
#define index_off_int	index_off4
#define offsetted_int	offsetted4
#define post_inc_int	post_inc4
#define pre_dec_int	pre_dec4
#define store_int	store4
#define test_set_int	test_set4

#define add_i	add_l
#define ADD_I	"add.l"
#define and_i	and_l
#define AND_I	"and.l"
#define asl_i	asl_l
#define ASL_I	"asl.l"
#define asr_i	asr_l
#define ASR_I	"asr.l"
#define clr_i	clr_l
#define CLR_I	"clr.l"
#define cmp_i	cmp_l
#define cmp2_i	cmp2_l
#define DEC	"sub.l #1,"
#define DIVS_I	"divs.l"
#define DIVU_I	"divu.l"
#define eor_i	eor_l
#define EOR_I	"eor.l"
#define INC	"add.l #1,"
#define lsr_i	lsr_l
#define LSR_I	"lsr.l"
#define move_i	move_l
#define MOVE_I	"move.l"
#define muls_i	muls_l
#define MULS_I	"muls.l"
#define mulu_i	mulu_l
#define MULU_I	"mulu.l"
#define neg_i	neg_l
#define NEG_I	"neg.l"
#define not_i	not_l
#define NOT_I	"not.l"
#define or_i	or_l
#define OR_I	"or.l"
#define rol_i	rol_l
#define ROL_I	"rol.l"
#define ror_i	ror_l
#define ROR_I	"ror.l"
#define sub_i	sub_l
#define SUB_I	"sub.l"
#define tst_i	tst_l

#endif
