#include "e.h"

 exa .1			; global Pascal variables
 exp $PROC
 exp $PROC1
 exp $PROC2
 exp $PROC3
 exp $PROCH
 exp $CLPASC1
 exp $CLPASC2
 exp $CLPASC5
 exp $CLRDSTR
 exp $CL68
 exp $FUNC68
 exp $CALLPASC

 pro $PROC,0
 LFL SZADDR+SZADDR	; load environment, static link for procedure
 LFL SZADDR		; load address of code
 ret SZPROC
 end 0

 pro $PROC1,0
 LFL SZADDR+SZADDR
 LFL SZADDR
 ret SZPROC
 end 0

 pro $PROC2,0
 LFL SZADDR+SZADDR
 LFL SZADDR
 ret SZPROC
 end 0

 pro $PROC3,0
 LFL SZADDR+SZADDR
 LFL SZADDR
 ret SZPROC
 end 0

 pro $PROCH,0
 LFL SZADDR+SZADDR
 LFL SZADDR
 ret SZPROC
 end 0

 pro $CLPASC1,SZWORD
 loc PASCALSTAMP
 stl -SZWORD
 lal SZADDR				; load base address of params (source)
 loc SZADDR+SZPROC
 los SZWORD
 cai
 ret 0
 end SZWORD

 pro $CLPASC2,SZWORD
 loc PASCALSTAMP
 stl -SZWORD				; set frame stamp as pascal
 lal SZADDR				; load base address of params (source)
 loc SZADDR+SZADDR+SZPROC
 los SZWORD
 cai					; call proc, params & static link set
 ret 0
 end SZWORD

#define P5PARAMSPACE SZADDR+SZADDR+SZWORD+SZWORD+SZADDR+SZPROC

 pro $CLPASC5,SZWORD
 loc PASCALSTAMP
 stl -SZWORD				; set frame stamp as pascal
 lal SZADDR				; load base address of params (source)
 loc P5PARAMSPACE
 los SZWORD
 cai
 ret 0
 end SZWORD

#define PRDSTRSPACE SZADDR+SZADDR+16+SZADDR+SZADDR+SZPROC

 pro $CLRDSTR,SZWORD
 loc PASCALSTAMP
 stl -SZWORD				; set frame stamp as pascal
 lal SZADDR				; load base address of params (source)
 loc PRDSTRSPACE
 los SZWORD
 cai
 ret 0
 end SZWORD

 pro $CL68,SZWORD
 loc PASCALSTAMP
 stl -SZWORD
 LFL SZADDR				; OBJECTP parameter
 LLC 0					; bitpattern
 loc 1					; locrg
 LFL SZADDR+SZADDR+SZADDR		; procbl
 dup SZADDR
 LFL SZADDR+SZADDR			; env
 exg SZADDR
 loi SZADDR				; XBASE
 cai
 ret 0
 end SZWORD

 pro $FUNC68,SZWORD
 loc PASCALSTAMP
 stl -SZWORD
 LFL SZADDR				; OBJECTP parameter
 LLC 0					; bitpattern
 loc 1					; locrg
 LFL SZADDR+SZADDR+SZADDR		; procbl
 dup SZADDR
 LFL SZADDR+SZADDR			; env
 exg SZADDR
 loi SZADDR				; XBASE
 cai
 ret SZWORD
 end SZWORD


 pro $CALLPASC,SZWORD ; +SZADDR+SZWORD
 loc PASCALSTAMP
 stl -SZWORD
 lal SZADDR+SZADDR+SZWORD+SZLONG        ; address of first (A68) parameter
 loe .1+SZWORD+SZADDR			; PASCPARAMS
 los SZWORD
 lae .1+SZWORD+SZADDR+SZWORD		; address of PASCPROC
 loi SZPROC				; PASCPROC
 cai
 ret 0
 end SZWORD ; +SZADDR+SZWORD
