#
 mes 2,EM_WSIZE,EM_PSIZE
#if EM_WSIZE<4
#define STATUS_SIZE 2
#else
#define STATUS_SIZE EM_WSIZE
#endif
 exp $_wait
 pro $_wait,0
 loc 7
 mon
 zne *1
 lal 0
 loi EM_PSIZE
 zer EM_PSIZE
 cms EM_PSIZE
 zeq *2
#if EM_WSIZE==1
 lal 0
 loi EM_PSIZE
 sti 2                  ; 2 bytes, not one int!
#else
 sil 0
#endif
 ret EM_WSIZE
2
 asp STATUS_SIZE
 ret EM_WSIZE
1
 ste errno
 loc -1
 ret EM_WSIZE
 end
