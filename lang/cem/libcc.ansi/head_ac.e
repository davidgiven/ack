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

 mes 2,_EM_WSIZE,_EM_PSIZE

 exa environ
 exa _end
 exa _penvp
_penvp
 bss _EM_PSIZE,0,0

 exp $_m_a_i_n
 pro $_m_a_i_n,0
 loc 1336	; EIOVFL | EIUND | ECONV | EFOVFL | EFUNFL
 sim            ; ignored
 lal _EM_WSIZE+_EM_PSIZE
 loi _EM_PSIZE
 lae _penvp
 sti _EM_PSIZE
/* Now test if environ is our own environ, or some user defined environ.
 * First test if environ < _end.  This is done for separate I&D systems.
 */
 lae environ
 lae _end
 cmp
 zge *1
/* Now environ < end, which means that we can derefence it without trouble
 * on separate I&D systems.
 */
 lae environ
 loi 2
 loc 2
 loc _EM_WSIZE
 cii
 loc 21331	/*  == 0x5353 */
 bne *1
/* environ contains the magic value. Assume it's our own environ */
 lae _penvp
 loi _EM_PSIZE
 lae environ
 sti _EM_PSIZE
1
#if unix && ! (em22 || em24 || em44)
 lpi $_ctch_
 sig
 asp _EM_PSIZE
#endif
 lal _EM_WSIZE+_EM_PSIZE
 loi _EM_PSIZE
 lal _EM_WSIZE
 loi _EM_PSIZE
 lal 0
 loi _EM_WSIZE
 cal $main
 asp _EM_WSIZE+_EM_PSIZE+_EM_PSIZE
 lfr _EM_WSIZE
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
 cal $_getpid
 lfr _EM_WSIZE
 cal $_kill
 asp 2*_EM_WSIZE
3
 lpi $_ctch_
 sig
 asp _EM_PSIZE
 rtt
 end 0
#endif
