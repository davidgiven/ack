#
; $Id$
;  (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
; 
;           This product is part of the Amsterdam Compiler Kit.
; 
;  Permission to use, sell, duplicate or disclose this software must be
;  obtained in writing. Requests for such permissions may be sent to
; 
;       Dr. Andrew S. Tanenbaum
;       Wiskundig Seminarium
;       Vrije Universiteit
;       Postbox 7161
;       1007 MC Amsterdam
;       The Netherlands
; 

/* Author: J.W. Stevenson */


 mes 2,EM_WSIZE,EM_PSIZE

#define TARLB   0
#define DESCR   EM_PSIZE

#define NEWPC   0
#define SAVSP   EM_PSIZE

#define D_PC    0
#define D_SP    EM_PSIZE
#define D_LB    EM_PSIZE+EM_PSIZE

#define LOCLB   -EM_PSIZE

; _gto is called with two arguments:
;       - pointer to the label descriptor (DESCR)
;       - local base (LB) of target procedure (TARLB)
; the label descriptor contains two items:
;       - label address i.e. new PC (NEWPC)
;       - offset in target procedure frame (SAVSP)
; using this offset and the LB of the target procedure, the address of
; of local variable of the target procedure is constructed.
; the target procedure must have stored the correct target SP there.

descr
 bss 3*EM_PSIZE,0,0

 exp $_gto
 pro $_gto,EM_PSIZE
 lal DESCR
 loi EM_PSIZE
 adp NEWPC
 loi EM_PSIZE
 lae descr+D_PC
 sti EM_PSIZE
 lal TARLB
 loi EM_PSIZE
 zer EM_PSIZE
 cmp
 zeq *1
 lal TARLB
 loi EM_PSIZE
 bra *2
1
 lae _m_lb
 loi EM_PSIZE
2
 lal LOCLB
 sti EM_PSIZE
 lal LOCLB
 loi EM_PSIZE
 lal DESCR
 loi EM_PSIZE
 adp SAVSP
 loi EM_WSIZE           ; or EM_PSIZE ?
 ads EM_WSIZE           ; or EM_PSIZE ?
 loi EM_PSIZE
 lae descr+D_SP
 sti EM_PSIZE
 lal LOCLB
 loi EM_PSIZE
 lae descr+D_LB
 sti EM_PSIZE
 gto descr
 end ?
