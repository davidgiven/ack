/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

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
