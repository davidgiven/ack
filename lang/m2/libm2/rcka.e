#
;
; (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
; See the copyright notice in the ACK home directory, in the file "Copyright".
;
;
; Module:	range checks for INTEGER, now for array indexing
; Author:	Ceriel J.H. Jacobs
; Version:	$Header$
;
#include <em_abs.h>

 mes 2,EM_WSIZE,EM_PSIZE
 exp $rcka
 pro $rcka, 0

 lol EM_PSIZE
 zlt *1		; lower bound always 0
 lol EM_PSIZE
 lal 0
 loi EM_PSIZE
 adp EM_WSIZE
 loi EM_WSIZE
 cmi EM_WSIZE
 zgt *1
 ret 0
1
 loc EARRAY
 trp
 ret 0
 end 0
