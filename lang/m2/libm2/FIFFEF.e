#
 mes 2,EM_WSIZE,EM_PSIZE

#define ARG1    0
#define ARG2    EM_FSIZE
#define IRES    2*EM_FSIZE

; FIFFEF_FIF is called with three parameters:
;       - address of integer part result (IRES)
;       - float two (ARG2)
;       - float one (ARG1)
; and returns an EM_FSIZE-byte floating point number
; Definition:
;	PROCEDURE FIF(ARG1, ARG2: REAL; VAR IRES: REAL) : REAL;

 exp $FIFFEF_FIF
 pro $FIFFEF_FIF,0
 lal 0
 loi 2*EM_FSIZE
 fif EM_FSIZE
 lal IRES
 loi EM_PSIZE
 sti EM_FSIZE
 ret EM_FSIZE
 end ?

#define FARG    0
#define ERES    EM_FSIZE

; FIFFEF_FEF is called with two parameters:
;       - address of base 2 exponent result (ERES)
;       - floating point number to be split (FARG)
; and returns an EM_FSIZE-byte floating point number (the mantissa)
; Definition:
;	PROCEDURE FEF(FARG: REAL; VAR ERES: integer): REAL;

 exp $FIFFEF_FEF
 pro $FIFFEF_FEF,0
 lal FARG
 loi EM_FSIZE
 fef EM_FSIZE
 lal ERES
 loi EM_PSIZE
 sti EM_WSIZE
 ret EM_FSIZE
 end ?

 exp $FIFFEF
 pro $FIFFEF,0
 ret 0
 end ?
