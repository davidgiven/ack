#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $brk
 pro $brk,0
 lal 0
 loi EM_PSIZE
 str 2          ; The - possibly - occurring traps should be caught
 zer EM_PSIZE
 ret EM_PSIZE
 end
