#
 mes 2,EM_WSIZE,EM_PSIZE
 exp $lseek
 pro $lseek,0
 lal 0
 loi 4*EM_WSIZE
 loc 19
 mon
 zeq *1
 ste errno
#if EM_WSIZE==1
 ldc -1
 loc 2
 loc 4
 cii
#endif
#if EM_WSIZE==2
 ldc -1
#endif
#if EM_WSIZE==4
 loc -1
#endif
1
#if EM_WSIZE==1
 ret 4*EM_WSIZE
#endif
#if EM_WSIZE==2
 ret 2*EM_WSIZE
#endif
#if EM_WSIZE==4
 ret EM_WSIZE
#endif
 end
