#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _gethostid
_gethostid: SYSTEM(SYS_gethostid)
