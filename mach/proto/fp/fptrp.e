#

 mes 2,EM_WSIZE,EM_PSIZE

#define TRAP    0

; _fptrp is called with one parameter:
;       - trap number (TRAP)

 exp $_fptrp
 pro $_fptrp,0
 lol TRAP
 trp
 ret 0
 end ?
