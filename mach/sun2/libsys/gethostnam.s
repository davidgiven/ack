#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _gethostname
_gethostname: SYSTEM(SYS_gethostname)
