#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _getsockname
_getsockname: SYSTEM(SYS_getsockname)
