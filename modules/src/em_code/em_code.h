/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#include "em_mesX.h"
#include "em_arith.h"

void C_ms_com(char *);
void C_ms_ego(int, arith, arith, int);
void C_ms_emx( arith, arith);
void C_ms_err(void);
void C_ms_flt(void);
void C_ms_gto(void);
void C_ms_opt(void);
void C_ms_par(arith);
void C_ms_reg(arith, arith, int, int);
void C_ms_src(int, char *);
void C_ms_stb_cst(char *, int, int, arith);
void C_ms_stb_dlb(char *, int, int, label, arith);
void C_ms_stb_dnam(char *, int, int, char *, arith);
void C_ms_stb_ilb(char *, int, int, label);
void C_ms_stb_pnam(char *, int, int, char *);
void C_ms_std(char *, int, int);



#ifdef PEEPHOLE
#include "em_codeO.h"
#include "emO_code.h"
#else
#ifdef CODE_EXPANDER
#include "em_codeCE.h"
#else
#include "em_codeEK.h"
#endif
#endif
