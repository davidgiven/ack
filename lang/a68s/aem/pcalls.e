#define SZWORD EM_WSIZE
#define SZADDR EM_PSIZE

#if SZADDR==SZWORD
#define LOAD lol
#define STORE stl
#else
#define LOAD ldl
#define STORE sdl
#endif

 mes 2,SZWORD,SZADDR

 exp $PROCENTR
 exp $PROCEXIT
 exp $ENCAPS
 exp $ABORT
 exp $TRAP

 pro $ABORT,0
 loc 1
 cal $_hlt
 asp SZWORD
 ret 0
 end 0

 pro $ENCAPS,0
 LOAD SZADDR
 LOAD 0
 LOAD SZADDR+SZADDR+SZADDR
 LOAD SZADDR+SZADDR
 cal  $encaps
 asp  SZADDR+SZADDR+SZADDR+SZADDR
 ret  0
 end  0

 pro $TRAP,0
 lol 0
 cal $trap
 asp SZWORD
 ret 0
 end 0

 pro $PROCENTR,0
 LOAD 0
 cal  $procentry
 asp  SZADDR
 ret  0
 end  0

 pro $PROCEXIT,0
 LOAD 0
 cal  $procexit
 asp  SZADDR
 ret  0
 end  0
