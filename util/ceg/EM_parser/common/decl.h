#define SEGBSS		0
#define SEGHOL		1
#define SEGCON		2
#define SEGROM		3
#define SEGTXT		4
#define NOSEG		5
#define UNKNOWN		(-1)


#define TRUE		1
#define	FALSE		0


#define STRING		0
#define LABEL		1
#define ARITH		2
#define INT		3


#define MAX_ARGS	4

#define NO_ARGS		0
#define C_ARG		1
#define D_ARG		2
#define F_ARG		3
#define L_ARG		4
#define N_ARG		5
#define O_ARG		6
#define R_ARG		7
#define S_ARG		8
#define Z_ARG		9
#define W_ARG		10
#define EXTERNAL	11
#define EXTERNAL_DNAM	12
#define EXTERNAL_DLB	13
#define P_ARG		14
#define B_ARG		15
#define STRING_ARITH	16
#define BSS_EXT_DNAM	17
#define BSS_EXT_DLB	18
#define BSS_STR_AR	19
#define BSS_W_ARG	20
#define BSS_PNAM	21
#define BSS_ILB		22
#define ID_N_ARG	23
#define ARITH_ARITH	24
#define EXT_DNAM 	25
#define EXT_DLB		26
#define ONE_INT		27
#define _STRING		28


typedef struct {
		char *header;
		int arg_type[MAX_ARGS];		/* Zoals ze eruit gaan!! */
		char *arg_id[MAX_ARGS], *arg_conv[MAX_ARGS];
	} t_C_info;

typedef struct {
		char *lexeme;
		int C_instr_type;
	} t_token;


extern t_C_info C_info[], *C_instr_info;
extern int storage_default[4][3], segment;
