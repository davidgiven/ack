#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _profil
_profil: SYSTEM(SYS_profil)
