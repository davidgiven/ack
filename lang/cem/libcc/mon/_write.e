#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $_write
 pro $_write,0
 lol EM_WSIZE+EM_PSIZE
 loc EM_WSIZE
 loc EM_PSIZE
 ciu
 lal 0
 loi EM_WSIZE+EM_PSIZE
 loc 4
 mon
 zne *1
 loc EM_PSIZE
 loc EM_WSIZE
 cui
 bra *2
1
 ste errno
 loc -1
2
 ret EM_WSIZE
 end
