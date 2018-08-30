#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $execv
 pro $execv,0
 lae environ
 loi EM_PSIZE
 lal 0
 loi 2*EM_PSIZE
 loc 59
 mon
 ste errno
 loc -1
 ret EM_WSIZE
 end
