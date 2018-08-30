#
 mes 2,EM_WSIZE,EM_PSIZE
 exa par_uid
par_uid
 bss EM_WSIZE,0,0

 exp $fork
 pro $fork,0
 loc 2
 mon
 zeq *1
 ste errno
 loc -1
 ret EM_WSIZE
1
 zeq *2
 ste par_uid
 loc 0
2
 ret EM_WSIZE
 end
