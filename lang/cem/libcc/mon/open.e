#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $open
 pro $open,0
 lal 0
 loi EM_WSIZE+EM_PSIZE
 loc 5
 mon
 zeq *1
 ste errno
 loc -1
1
 ret EM_WSIZE
 end
