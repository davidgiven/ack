#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $chdir
 pro $chdir,0
 lal 0
 loi EM_PSIZE
 loc 12
 mon
 zne *1
 loc 0
 ret EM_WSIZE
1
 ste errno
 loc -1
 ret EM_WSIZE
 end
