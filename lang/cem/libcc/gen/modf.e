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

 mes 2,EM_WSIZE,EM_PSIZE
#ifndef NOFLOAT
 exp $modf
 pro $modf,0
 lal 0
 loi EM_DSIZE
 loc 1
 loc EM_WSIZE
 loc EM_DSIZE
 cif
 fif EM_DSIZE
 lal EM_DSIZE
 loi EM_PSIZE
 sti EM_DSIZE
 ret EM_DSIZE
 end
#endif
