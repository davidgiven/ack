#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $umask
 pro $umask,0
 lol 0
 loc 60
 mon
 ret EM_WSIZE
 end
