#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $sbrk
 pro $sbrk,0
 lor 2
 lor 2
 lol 0
 ads EM_WSIZE
 str 2          ; The - possibly - occurring traps should be caught
 ret EM_PSIZE
 end
