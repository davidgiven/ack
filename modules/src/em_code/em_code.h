/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#include "em_mesX.h"
#include "em_arith.h"
#include "ansi.h"

_PROTOTYPE(void C_ms_com, (char *));
_PROTOTYPE(void C_ms_ego, (int, arith, arith, int));
_PROTOTYPE(void C_ms_emx, ( arith, arith));
_PROTOTYPE(void C_ms_err, (void));
_PROTOTYPE(void C_ms_flt, (void));
_PROTOTYPE(void C_ms_gto, (void));
_PROTOTYPE(void C_ms_opt, (void));
_PROTOTYPE(void C_ms_par, (arith));
_PROTOTYPE(void C_ms_reg, (arith, arith, int, int));
_PROTOTYPE(void C_ms_src, (int, char *));
_PROTOTYPE(void C_ms_stb_cst, (char *, int, int, arith));
_PROTOTYPE(void C_ms_stb_dlb, (char *, int, int, label, arith));
_PROTOTYPE(void C_ms_stb_dnam, (char *, int, int, char *, arith));
_PROTOTYPE(void C_ms_stb_ilb, (char *, int, int, label));
_PROTOTYPE(void C_ms_stb_pnam, (char *, int, int, char *));
_PROTOTYPE(void C_ms_std, (char *, int, int));



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
