#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $_fstat
 pro $_fstat,0
 lal 0
 loi EM_WSIZE+EM_PSIZE
 loc 28
 mon
 zne *1
 loc 0
 ret EM_WSIZE
1
 ste errno
 loc -1
 ret EM_WSIZE
 end
