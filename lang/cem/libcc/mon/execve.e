#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $execve
 pro $execve,0
 lal 0
 loi 3*EM_PSIZE
 loc 59
 mon
 ste errno
 loc -1
 ret EM_WSIZE
 end
