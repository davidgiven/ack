#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $_dup
 pro $_dup,0
 lol 0
 dup EM_WSIZE
 loc 41
 mon
 zeq *1
 ste errno
 loc -1
1
 ret EM_WSIZE
 end
