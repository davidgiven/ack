#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $chown
 pro $chown,0
 lal 0
 loi EM_PSIZE+2*EM_WSIZE
 loc 16
 mon
 zne *1
 loc 0
 ret EM_WSIZE
1
 ste errno
 loc -1
 ret EM_WSIZE
 end
