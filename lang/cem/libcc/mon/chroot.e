#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $chroot
 pro $chroot,0
 lal 0
 loi EM_PSIZE
 loc 61
 mon
 zne *1
 loc 0
 ret EM_WSIZE
1
 ste errno
 loc -1
 ret EM_WSIZE
 end
