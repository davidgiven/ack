#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _nfssvc
_nfssvc: SYSTEM(SYS_nfssvc)
