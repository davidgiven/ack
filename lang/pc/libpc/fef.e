#
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
