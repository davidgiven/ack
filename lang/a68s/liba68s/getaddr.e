#define SZWORD EM_WSIZE
#define SZADDR EM_PSIZE

#if SZWORD==SZADDR
#define LOAD lol
#define STORE stl
#else
#define LOAD ldl
#define STORE sdl
#endif

 mes 2,SZWORD,SZADDR

 exp $GETADDRE
 pro $GETADDRE,0
 LOAD SZADDR	; load param (adress of variable) (1st after static link)
 ret SZADDR	; return address
 end 0
