#
 mes 2,EM_WSIZE,EM_PSIZE

; $Id$

#define ARG1    0
#define ARG2    EM_DSIZE
#define IRES    2*EM_DSIZE

; _fif is called with three parameters:
;       - address of integer part result (IRES)
;       - float two (ARG2)
;       - float one (ARG1)
; and returns an EM_DSIZE-byte floating point number

 exp $_fif
 pro $_fif,0
 lal 0
 loi 2*EM_DSIZE
 fif EM_DSIZE
 lal IRES
 loi EM_PSIZE
 sti EM_DSIZE
 ret EM_DSIZE
 end ?
