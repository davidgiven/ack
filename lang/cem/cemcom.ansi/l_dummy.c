/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

/*
 *The following functions are hacked to null-functions (i.e. they
 * do nothing). This needs another solution in the future.
 */
#include	"lint.h"

#ifdef	LINT

#include	"arith.h"
#include	"label.h"

C_close(){}
int C_busy(){return 0;}


/* More routines */
/* ARGSUSED */
CC_bhcst(ps_xxx,n,w,i) arith n,w; {}
/* ARGSUSED */
CC_crcst(ps_xxx,v) arith v; {}
/* ARGSUSED */
CC_crdlb(ps_xxx,v,s) label v; arith s; {}
/* ARGSUSED */
CC_crdnam(ps_xxx,v,s) char *v; arith s; {}
/* ARGSUSED */
CC_crfcon(ps_xxx,v,s) char *v; arith s; {}
/* ARGSUSED */
CC_cricon(ps_xxx,v,s) char *v; arith s; {}
/* ARGSUSED */
CC_crilb(ps_xxx,v) label v; {}
/* ARGSUSED */
CC_crpnam(ps_xxx,v) char *v; {}
/* ARGSUSED */
CC_crscon(ps_xxx,v,s) char *v; arith s; {}
/* ARGSUSED */
CC_crucon(ps_xxx,v,s) char *v; arith s; {}
/* ARGSUSED */
CC_cst(l) {}
/* ARGSUSED */
CC_dfdlb(l) label l; {}
/* ARGSUSED */
CC_dfdnam(s) char *s; {}
/* ARGSUSED */
CC_dfilb(l) label l; {}
/* ARGSUSED */
CC_end(l) arith l; {}
CC_msend() {}
/* ARGSUSED */
CC_msstart(ms) {}
/* ARGSUSED */
CC_opcst(op_xxx,c) arith c; {}
/* ARGSUSED */
CC_opdlb(op_xxx,g,o) label g; arith o; {}
/* ARGSUSED */
CC_opilb(op_xxx,b) label b; {}
/* ARGSUSED */
CC_oppnam(op_xxx,p) char *p; {}
/* ARGSUSED */
CC_pronarg(s) char *s; {}
/* ARGSUSED */
CC_psdlb(ps_xxx,l) label l; {}
/* ARGSUSED */
CC_psdnam(ps_xxx,s) char *s; {}
/* ARGSUSED */
CC_pspnam(ps_xxx,s) char *s; {}
/* ARGSUSED */
CC_scon(v,s) char *s; {}
#endif	LINT
