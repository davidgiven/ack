/* $Header$ */

struct e_instr {
	int	em_type;		/* Type of this instr */
#define EM_MNEM		256		/* A machine instruction */
#define	EM_PSEU		257		/* A pseudo */
#define EM_STARTMES	258		/* Start of a MES pseudo */
#define EM_MESARG	259		/* A member in a MES list */
#define EM_ENDMES	260		/* End of a MES pseudo */
#define EM_DEFILB	261		/* An instruction label definition */
#define EM_DEFDLB	262		/* A numeric data label definition */
#define EM_DEFDNAM	263		/* A non-numeric data label def */
#define EM_ERROR	264		/* Recoverable error */
#define EM_FATAL	265		/* Unrecoverable error */
	union {
	      struct {
	           int	emus_opcode;	/* Opcode of instruction */
	           struct e_args *emus_args;	/* Arguments of instruction */
	      } emu_mp;
	      label	emu_deflb;	/* Numeric label definition */
	      char	*emu_defdnam;	/* Non-numeric label definition */
	      struct e_args	*emu_arg;	/* For an argument */
	} em_i; 
#define em_opcode	em_i.emu_mp.emus_opcode
#define em_args		em_i.emu_mp.emus_args
#define em_deflb	em_i.emu_deflb
#define em_defdnam	em_i.emu_defdnam
#define em_arg		em_i.emu_arg
};

struct e_args {
	struct e_args	*em_next;	/* Next argument */
	short	em_argtype;		/* Type of this argument */
	union {
	       arith	emu_cst;		/* A constant */
	       label	emu_ilb;		/* An instruction label */
	       char	*emu_pnam;	/* A procedure name (not including '$') */
	       struct {
	              label	emus_dlb;
	              arith	emus_noff;
	       } emu_ndlb;		/* Numeric data label + offset */
	       struct {
	              char	*emus_dnam;
	              arith	emus_soff;
	       } emu_sdlb;		/* String data label + offset */
	       struct {
	              char	*emus_str;
	              arith	emus_size;
	       } emu_con;			/* An scon, icon, ucon or fcon */
	} em_value;
#define em_cst		em_value.emu_cst
#define em_ilb		em_value.emu_ilb
#define em_pnam		em_value.emu_pnam
#define em_dlb		em_value.emu_ndlb.emus_dlb
#define em_noff		em_value.emu_ndlb.emus_noff
#define em_dnam		em_value.emu_sdlb.emus_dnam
#define em_soff		em_value.emu_sdlb.emus_soff
#define em_str		em_value.emu_con.emus_str
#define em_size		em_value.emu_con.emus_size
};

extern char *EM_error;
extern unsigned int EM_lineno;
extern char *EM_filename;
extern int EM_wordsize, EM_pointersize;
