#
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

 exp $m_a_i_n
 pro $m_a_i_n,0
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
