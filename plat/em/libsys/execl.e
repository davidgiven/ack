#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $execl
 pro $execl,0
 lae environ
 loi EM_PSIZE
 lal EM_PSIZE
 lal 0
 loi EM_PSIZE
 loc 59
 mon
 ste errno
 loc -1
 ret EM_WSIZE
 end
