#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _getsockopt
_getsockopt: SYSTEM(SYS_getsockopt)
