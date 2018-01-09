#define D(e) .define e; e

! Define various EM trap numbers needed by mach/i386/libem.
! Note that these are *not* ANSI C errnos.

D(ERANGE)         = 1
D(ESET)           = 2
D(EIDIVZ)         = 6
D(EHEAP)          = 17
D(EILLINS)        = 18
D(EODDZ)          = 19
D(ECASE)          = 20
D(EBADMON)        = 25
