#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $_ioctl
 pro $_ioctl,0
 lal 0
 loi EM_PSIZE+2*EM_WSIZE
 loc 54
 mon
 zne *1
 loc 0
 ret EM_WSIZE
1
 ste errno
 loc -1
 ret EM_WSIZE
 end
