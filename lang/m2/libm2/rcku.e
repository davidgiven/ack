#
;
; (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
; See the copyright notice in the ACK home directory, in the file "Copyright".
;
;
; Module:	range checks for CARDINAL
; Author:	Ceriel J.H. Jacobs
; Version:	$Header$
;
#include <em_abs.h>

 mes 2,EM_WSIZE,EM_PSIZE
 exp $rcku
 pro $rcku, 0

 lal EM_PSIZE
 loi EM_WSIZE
 lal 0
 loi EM_PSIZE
 loi EM_WSIZE
 cmu EM_WSIZE
 zlt *1
 lal EM_PSIZE
 loi EM_WSIZE
 lal 0
 loi EM_PSIZE
 adp EM_WSIZE
 loi EM_WSIZE
 cmu EM_WSIZE
 zgt *1
 ret 0
1
 loc ERANGE
 trp
 ret 0
 end 0
