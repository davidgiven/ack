#
 mes 2,EM_WSIZE,EM_PSIZE
; Save the line where the error occurred
 exp $_setline
 pro $_setline,0
 exa _erlsym
 loe 0
 ste _erlsym
 ret 0
 end
