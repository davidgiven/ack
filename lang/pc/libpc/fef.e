#
; $Id$
;
; (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
;
;          This product is part of the Amsterdam Compiler Kit.
;
; Permission to use, sell, duplicate or disclose this software must be
; obtained in writing. Requests for such permissions may be sent to
;
;      Dr. Andrew S. Tanenbaum
;      Wiskundig Seminarium
;      Vrije Universiteit
;      Postbox 7161
;      1007 MC Amsterdam
;      The Netherlands
;
;

 mes 2,EM_WSIZE,EM_PSIZE

#define FARG    0
#define ERES    EM_DSIZE

; _fef is called with two parameters:
;       - address of exponent result (ERES)
;       - floating point number to be split (FARG)
; and returns an EM_DSIZE-byte floating point number

 exp $_fef
 pro $_fef,0
 lal FARG
 loi EM_DSIZE
 fef EM_DSIZE
 lal ERES
 loi EM_PSIZE
 sti EM_WSIZE
 ret EM_DSIZE
 end ?
