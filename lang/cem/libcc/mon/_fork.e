#
 mes 2,EM_WSIZE,EM_PSIZE

 exp $_fork
 pro $_fork,0
 loc 2
 mon
 zeq *1
 ste errno
 loc -1
 ret EM_WSIZE
1
 zeq *2
 asp EM_WSIZE
 loc 0
2
 ret EM_WSIZE
 end
