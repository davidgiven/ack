#include "em_parser.h"

#define NOSEG		5
#define UNKNOWN		(-1)


#define TRUE		1
#define	FALSE		0


#define MAX_ARGS	4

/* All the different types of C_INSTR's, based on types of the arguments. */
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
		char *header;		 /* The declaration of parameters for
					  * each type of C_INSTR.
					  */
		int arg_type[MAX_ARGS];	 /* Type of the $i's after the
					  * conversions ( extnd_name(), etc.).
					  */
		char *arg_id[MAX_ARGS],	 /* Names of the parameters, to be used
					  * left of the '==>' sign.
					  */
		     *arg_conv[MAX_ARGS];/* Names of the parameters, to be used
					  * right of the '==>' sign.
					  */
	} t_C_info;


typedef struct {
		char *lexeme;
		int C_instr_type;
	} t_token;


extern t_C_info C_info[], *C_instr_info;
extern int storage_default[4][3], segment;
