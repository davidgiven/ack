#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $execle
 pro $execle,0
 lal EM_PSIZE
1
 dup EM_PSIZE
 loi EM_PSIZE
 zer EM_PSIZE
 cmp
 zeq *2
 adp EM_PSIZE
 bra *1
2
 adp EM_PSIZE
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
