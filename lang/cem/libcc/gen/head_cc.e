#
#include "em_abs.h"
/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
 *
 */

/* Author: E.G. Keizer */

 mes 2,EM_WSIZE,EM_PSIZE

 exa environ
 exa _penvp
environ
 bss EM_PSIZE,0,0
_penvp
 bss EM_PSIZE,0,0

 exp $_m_a_i_n
 pro $_m_a_i_n,0
#if EM_WSIZE==1
 ldc 1336	; EIOVFL | EIUND | ECONV | EFOVFL | EFUNFL
#else
 loc 1336
#endif
 sim            ; ignored
 lal EM_WSIZE+EM_PSIZE
 loi EM_PSIZE
 lae environ
 sti EM_PSIZE
 lae environ
 loi EM_PSIZE
 lae _penvp
 sti EM_PSIZE
#if unix && ! (em22 || em24 || em44)
 lpi $_ctch_
 sig
 asp EM_PSIZE
#endif
 lal EM_WSIZE+EM_PSIZE
 loi EM_PSIZE
 lal EM_WSIZE
 loi EM_PSIZE
 lal 0
 loi EM_WSIZE
 cal $main
 asp EM_WSIZE+EM_PSIZE+EM_PSIZE
 lfr EM_WSIZE
 cal $exit
 end

#if unix && ! (em22 || em24 || em44)
 exp $_ctch_
 pro $_ctch_,0
 lol 0
 loc EIDIVZ
 beq *1
 lol 0
 loc EFDIVZ
 beq *1
 lol 0
 trp
 bra *3
1
 loc 8
 cal $getpid
 lfr EM_WSIZE
 cal $kill
 asp 2*EM_WSIZE
3
 lpi $_ctch_
 sig
 asp EM_PSIZE
 rtt
 end 0
#endif
